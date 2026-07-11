// Pin configuration
#define ANEMOMETER_PIN 15
#define SAMPLE_TIME 3000  // Sample every 3 seconds

// Pulse counting variables
volatile unsigned int pulseCount = 0;
volatile unsigned long lastPulseTime = 0;
unsigned long lastSampleTime = 0;

// Wind speed variables
float currentWindSpeed = 0.0;
float avgWindSpeed = 0.0;
float maxWindSpeed = 0.0;
float minWindSpeed = 999.0;
float lastRPS = 0.0;

// Averaging system
const int avgSamples = 5;
float windSpeedArray[avgSamples];
int avgIndex = 0;
bool arrayFilled = false;

// Polynomial calibration coefficients
const float POLY_A = -0.0181;
const float POLY_B = 1.3859;
const float POLY_C = 1.4055;

// Thresholds
const float DEAD_ZONE = 1.5;     // Below this = 0 m/s
const float MAX_REASONABLE = 50.0; // Above this = sensor error
const unsigned long PULSE_TIMEOUT = 5000; // 5s without pulse = no wind

// Statistics
unsigned long totalSamples = 0;
unsigned long errorCount = 0;

void IRAM_ATTR pulseCounter() {
  pulseCount++;
  lastPulseTime = millis();
}

void setup() {
  Serial.begin(115200);
  Serial.println("=== Anemometer Advanced Test ===");
  Serial.println("Enhanced with error handling & statistics");
  
  pinMode(ANEMOMETER_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ANEMOMETER_PIN), pulseCounter, FALLING);
  
  // Initialize arrays
  for (int i = 0; i < avgSamples; i++) {
    windSpeedArray[i] = 0.0;
  }
  
  lastSampleTime = millis();
  lastPulseTime = millis();
  
  Serial.println("Calibration: Polynomial (A=" + String(POLY_A, 4) + 
                " B=" + String(POLY_B, 4) + " C=" + String(POLY_C, 4) + ")");
  Serial.println("Dead zone: < " + String(DEAD_ZONE) + " m/s");
  Serial.println("Ready for measurement...\n");
}

void loop() {
  // Check if sampling time reached
  if (millis() - lastSampleTime >= SAMPLE_TIME) {
    calculateWindSpeed();
    displayResults();
    resetCounters();
  }
  
  // Check for sensor timeout (no pulse for too long)
  if (millis() - lastPulseTime > PULSE_TIMEOUT && pulseCount == 0) {
    // Probably no wind or sensor disconnected
    currentWindSpeed = 0.0;
  }
  
  delay(50); // Reduced delay for better responsiveness
}

void calculateWindSpeed() {
  // Safely disable interrupt
  detachInterrupt(digitalPinToInterrupt(ANEMOMETER_PIN));
  
  // Store pulse count for calculation
  unsigned int currentPulseCount = pulseCount;
  
  // Calculate RPS
  lastRPS = (float)currentPulseCount * 1000.0 / SAMPLE_TIME;
  
  // Apply polynomial calibration
  float rawSpeed = (POLY_A * lastRPS * lastRPS) + (POLY_B * lastRPS) + POLY_C;
  
  // Validation & filtering
  if (rawSpeed <= DEAD_ZONE) {
    currentWindSpeed = 0.0;
  } else if (rawSpeed > MAX_REASONABLE) {
    // Probable sensor error - use last valid reading
    currentWindSpeed = (avgWindSpeed > 0) ? avgWindSpeed : 0.0;
    errorCount++;
    Serial.println("WARNING: Unreasonable reading detected!");
  } else {
    currentWindSpeed = rawSpeed;
  }
  
  // Update statistics
  if (currentWindSpeed > maxWindSpeed) maxWindSpeed = currentWindSpeed;
  if (currentWindSpeed < minWindSpeed && currentWindSpeed > 0) minWindSpeed = currentWindSpeed;
  
  // Moving average calculation
  windSpeedArray[avgIndex] = currentWindSpeed;
  avgIndex = (avgIndex + 1) % avgSamples;
  
  // Mark array as filled after first cycle
  if (avgIndex == 0 && !arrayFilled) {
    arrayFilled = true;
  }
  
  // Calculate average (only use filled slots initially)
  int samplesToUse = arrayFilled ? avgSamples : avgIndex;
  avgWindSpeed = 0.0;
  for (int i = 0; i < samplesToUse; i++) {
    avgWindSpeed += windSpeedArray[i];
  }
  avgWindSpeed /= samplesToUse;
  
  totalSamples++;
  
  // Re-enable interrupt
  attachInterrupt(digitalPinToInterrupt(ANEMOMETER_PIN), pulseCounter, FALLING);
}

void displayResults() {
  Serial.println("=== Wind Measurement Results ===");
  Serial.printf("Sample #%lu | Pulse count (%ds): %u\n", 
                totalSamples, SAMPLE_TIME/1000, pulseCount);
  
  Serial.printf("Rotations/sec: %.3f RPS\n", lastRPS);
  Serial.printf("Current speed: %.2f m/s\n", currentWindSpeed);
  Serial.printf("Average speed: %.2f m/s (%.2f km/h)\n", 
                avgWindSpeed, avgWindSpeed * 3.6);
  
  // Statistics
  if (totalSamples > 1) {
    Serial.printf("Min/Max speed: %.2f / %.2f m/s\n", minWindSpeed, maxWindSpeed);
    Serial.printf("Error rate: %.1f%% (%lu errors)\n", 
                  (errorCount * 100.0 / totalSamples), errorCount);
  }
  
  Serial.printf("Wind condition: %s\n", getWindCondition(avgWindSpeed).c_str());
  
  // Sensor health check
  if (millis() - lastPulseTime > PULSE_TIMEOUT) {
    Serial.println("⚠️  No recent pulses detected");
  }
  
  Serial.println("--------------------------------\n");
}

void resetCounters() {
  pulseCount = 0;
  lastSampleTime = millis();
}

String getWindCondition(float speed) {
  // Enhanced Beaufort scale with more precision
  if (speed < 0.3) return "Tenang";
  else if (speed < 1.5) return "Sepoi-sepoi ringan";
  else if (speed < 3.3) return "Sepoi-sepoi sedang";
  else if (speed < 5.4) return "Angin pelan";
  else if (speed < 7.9) return "Angin sedang";
  else if (speed < 10.7) return "Angin agak kencang";
  else if (speed < 13.8) return "Angin kencang";
  else if (speed < 17.1) return "Angin keras";
  else if (speed < 20.7) return "Angin sangat keras";
  else return "Badai";
}

// Calibration helper function
void printCalibrationData() {
  Serial.println("\n=== Calibration Data Helper ===");
  Serial.println("RPS\tPredicted_Speed");
  for (float rps = 0; rps <= 10; rps += 0.5) {
    float speed = (POLY_A * rps * rps) + (POLY_B * rps) + POLY_C;
    Serial.printf("%.1f\t%.2f\n", rps, speed);
  }
  Serial.println("===============================\n");
}