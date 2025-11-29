const int LDR_PIN = 34;

// --- Your Calibration Values ---
const int LDR_MIN_READING = 0;      // Darkest value
const int LDR_MAX_READING = 2400;   // Your max value (under flashlight)
const int LUX_MAX_GUESS = 1000;     // Assumed Lux value at max reading

// --- 3-Level Thresholds ---
const int THRESHOLD_LOW = 800;
const int THRESHOLD_MID = 1600;

void setup() {
  Serial.begin(115200);
}

void loop() {
  // Read the raw analog value
  int ldrValue = analogRead(LDR_PIN);

  // 1. Map analog value to estimated Lux
  int constrainedVal = constrain(ldrValue, LDR_MIN_READING, LDR_MAX_READING);
  long approxLux = map(constrainedVal, LDR_MIN_READING, LDR_MAX_READING, 0, LUX_MAX_GUESS);

  // 2. Map raw value to 3 levels
  String lightLevel;
  if (ldrValue <= THRESHOLD_LOW) {
    lightLevel = "Redup";
  } else if (ldrValue <= THRESHOLD_MID) {
    lightLevel = "Sedang";
  } else {
    lightLevel = "Terang";
  }

  // Print the results
  Serial.print("LDR Value: ");
  Serial.print(ldrValue);
  Serial.print("  |  Level: ");
  Serial.print(lightLevel);
  Serial.print("  |  Approx Lux: ");
  Serial.print(approxLux);
  Serial.println(" Lux");

  delay(500);
}