#include <DHT.h>
#include <Wire.h>
#include <RTClib.h>
#include <BH1750.h>
#include <ArduinoJson.h>
#include <HardwareSerial.h>
#include <Adafruit_INA219.h>
#include <LiquidCrystal_I2C.h>

// ===== RTC SETUP =====
RTC_DS3231 rtc;

// ===== RAIN SENSOR CONFIG =====
const int RAIN_PIN = 14;
const float MM_PER_TIP = 0.2;
volatile int tipCount = 0;
volatile unsigned long lastTipTime = 0;
volatile int lastRainTipCount = 0;
float rainRateMmPerHour = 0.0;

// ===== WIND SENSOR CONFIG =====
const int ANEMOMETER_PIN = 15;
volatile unsigned long pulseCount = 0;
volatile unsigned long lastPulseTime = 0;
const int debounceTimeMicros = 5000;
float windSpeedKmPerHour = 0.0;
float windSpeedMs = 0.0;

// ===== LIGHT SENSOR =====
BH1750 lightSensor;
float lux = 0.0;

// ===== TEMPERATURE & HUMIDITY SENSOR =====
const int DHTPIN = 27;
const int DHTTYPE = DHT22;
DHT dht(DHTPIN, DHTTYPE);
float temperature = 0.0;
float humidity = 0.0;

// ===== POWER MONITORING =====
Adafruit_INA219 ina219Solar(0x40); // Solar panel monitor
float solBusVoltage = 0.0;
float solCurrent_mA = 0.0;
float solPower_mW = 0.0;

// Adafruit_INA219 ina219Battery(0x41); // Battery monitor
// float batBusVoltage = 0.0;
// float batCurrent_mA = 0.0;
// float batPower_mW = 0.0;

// ===== LCD DISPLAY =====
LiquidCrystal_I2C lcd(0x27, 20, 4);
int lcdPage = 0;

// ===== TIMING CONTROL =====
unsigned long lastWindUpdateTime = 0;
const unsigned long windUpdateInterval = 1500; // 1.5s

unsigned long lastSlowSensorUpdateTime = 0;
const unsigned long slowSensorUpdateInterval = 5000; // 5s

unsigned long lastPageSwitchTime = 0;
const unsigned long pageSwitchInterval = 7000; // 7s

// ===== SIM7600 CONFIG =====
#define ESP_RX 17
#define ESP_TX 16
HardwareSerial simSerial(1);

const char* APN = "indosatgprs";
// const char* SERVER_URL = "https://httpbin.org/post";
const char* SERVER_URL = "https://www.atamagri.app/api/iot/sensor-data";
const unsigned long POST_INTERVAL = 10000; // 10s
const String DEVICE_ID = "ESP32-001";

unsigned long lastPostTime = 0;
unsigned long postStartTime = 0;
bool isPosting = false;

// ===== INTERRUPT SERVICE ROUTINES =====
void IRAM_ATTR onTip() {
  if (millis() - lastTipTime > 250) { // Debounce 250ms
    tipCount++;
    lastTipTime = millis();
  }
}

void IRAM_ATTR anemometerISR() {
  if (micros() - lastPulseTime >= debounceTimeMicros) {
    pulseCount++;
    lastPulseTime = micros();
  }
}

// ===== SIM7600 FUNCTIONS =====
void sendATCommand(const String& cmd) {
  simSerial.println(cmd);
}

void clearBuffer() {
  while (simSerial.available()) {
    simSerial.read();
  }
}

void initModem() {
  Serial.print("Initializing SIM7600...");
  
  sendATCommand("AT"); delay(500);
  sendATCommand("AT+CPIN?"); delay(500); // Check SIM status
  sendATCommand("AT+CSQ"); delay(500); // Signal quality
  sendATCommand("AT+CREG?"); delay(500); // Network registration

  sendATCommand("AT+CGDCONT=1,\"IP\",\"" + String(APN) + "\""); delay(1000);
  
  sendATCommand("AT+CGATT=1"); delay(2000); // Attach to GPRS
  sendATCommand("AT+CGACT=1,1"); delay(4000); // Activate PDP context
  sendATCommand("AT+CGPADDR=1"); delay(1000); // Get IP address
  
  clearBuffer();
  Serial.println(" SIM7600 initialized");
}

void initHTTP() {
  sendATCommand("AT+HTTPTERM"); delay(500); // Terminate previous session
  sendATCommand("AT+HTTPINIT"); delay(1000); // Initialize HTTP
  
  sendATCommand("AT+HTTPPARA=\"CID\",1"); delay(200);
  sendATCommand("AT+HTTPPARA=\"URL\",\"" + String(SERVER_URL) + "\""); delay(200);
  sendATCommand("AT+HTTPPARA=\"CONTENT\",\"application/json\""); delay(200);
  
  clearBuffer();
}

void startHTTPPost(const String& payload) {
  if (isPosting) return;

  Serial.println("\n=== SENDING DATA ===");
  Serial.println("Payload: " + payload);

  // Set data length and timeout
  sendATCommand("AT+HTTPDATA=" + String(payload.length()) + ",10000");
  delay(300);
  
  // Send actual data
  simSerial.print(payload);
  delay(500);
  
  // Start HTTP POST
  sendATCommand("AT+HTTPACTION=1");

  isPosting = true;
  postStartTime = millis();
}

void checkPostStatus() {
  if (!isPosting) return;

  // Timeout check
  if (millis() - postStartTime > 15000) {
    Serial.println("POST TIMEOUT");
    isPosting = false;
    return;
  }

  String response;
  while (simSerial.available()) {
    response += char(simSerial.read());
  }

  if (response.indexOf("+HTTPACTION:") != -1) {
    if (response.indexOf(",200,") != -1) {
      Serial.println("POST SUCCESS");
    } else {
      Serial.println("POST FAILED");
      Serial.println("Response: " + response);
    }
    
    isPosting = false;
    
    // Read response data
    sendATCommand("AT+HTTPREAD=0,200");
    delay(500);
    clearBuffer();
  }
}

// ===== SENSOR UPDATE FUNCTIONS =====
void updateFastSensors() {
  // Wind speed calculation (atomic operation)
  detachInterrupt(digitalPinToInterrupt(ANEMOMETER_PIN));
  unsigned long pulses = pulseCount;
  pulseCount = 0;
  attachInterrupt(digitalPinToInterrupt(ANEMOMETER_PIN), anemometerISR, RISING);

  float elapsed = (millis() - lastWindUpdateTime) / 1000.0;
  windSpeedKmPerHour = (elapsed > 0) ? (pulses / elapsed) * 2.4 : 0; // Calculate wind speed in km/h
  float rps = (elapsed > 0) ? (pulses / elapsed) : 0; // Calculate RPS
  windSpeedMs = rps * 2.4 * 0.27778; // Convert to m/s

  // Light sensor reading
  lux = lightSensor.readLightLevel();
  if (lux < 0) lux = 0;
}

void updateSlowSensors() {
  // Rain rate calculation
  int newTips = tipCount - lastRainTipCount;
  lastRainTipCount = tipCount;
  rainRateMmPerHour = newTips * MM_PER_TIP * 720.0;  // 3600/5 = 720

  // Temperature and humidity
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  if (!isnan(h) && !isnan(t)) {
    humidity = h;
    temperature = t;
  }

  // Solar panel monitoring
  solBusVoltage = ina219Solar.getBusVoltage_V();
  solCurrent_mA = ina219Solar.getCurrent_mA();
  solPower_mW = ina219Solar.getPower_mW();

  // Battery monitoring
  // batBusVoltage = ina219Battery.getBusVoltage_V();
  // batCurrent_mA = ina219Battery.getCurrent_mA();
  // batPower_mW = ina219Battery.getPower_mW();
}

// ===== LCD DISPLAY FUNCTIONS =====
void printAt(int col, int row, const String& text, int width) {
  lcd.setCursor(col, row);
  lcd.print(text);
  
  // Clear remaining characters
  for (int i = text.length(); i < width; i++) {
    lcd.print(" ");
  }
}

void updateLCDPage(int page) {
  switch(page) {
    case 0: // WIND & RAIN PAGE
      lcd.setCursor(0, 0);
      lcd.print("=== WIND & RAIN ===");
      printAt(0, 1, "Wind: " + String(windSpeedMs, 1) + " m/s", 20);
      printAt(0, 2, "Wind: " + String(windSpeedKmPerHour, 1) + " km/h", 20);
      printAt(0, 3, "Rain: " + String(rainRateMmPerHour, 1) + " mm/h", 20);
      break;

    case 1: // WEATHER PAGE
      lcd.setCursor(0, 0);
      lcd.print("==== WEATHER ====");
      printAt(0, 1, "Temp : " + String(temperature, 1) + " \xDF""C", 20);
      printAt(0, 2, "Humid: " + String(humidity, 1) + " %", 20);
      printAt(0, 3, "Light: " + String((int)lux) + " lux", 20);
      break;

    case 2: // SOLAR PANEL PAGE
      lcd.setCursor(0, 0);
      lcd.print("=== SOLAR PANEL ===");
      printAt(0, 1, "Voltage: " + String(solBusVoltage, 2) + " V", 20);
      printAt(0, 2, "Current: " + String(solCurrent_mA, 0) + " mA", 20);
      printAt(0, 3, "Power  : " + String(solPower_mW / 1000, 2) + " W", 20);
      break;
    
    // case 3: // BATTERY PAGE
    //   lcd.setCursor(0, 0);
    //   lcd.print("==== BATTERY ====");
    //   printAt(0, 1, "Voltage: " + String(batBusVoltage, 2) + " V", 20);
    //   printAt(0, 2, "Current: " + String(batCurrent_mA, 0) + " mA", 20);
    //   printAt(0, 3, "Power  : " + String(batPower_mW / 1000, 2) + " W", 20);
    //   break;
  }
}

void updateLCD() {
  updateLCDPage(lcdPage);
}

// ===== DATA TRANSMISSION =====
void sendSensorData() {
  StaticJsonDocument<512> data; // Increased size for more data
  
  // Get current timestamp
  DateTime now = rtc.now();
  char timeStamp[25];
  sprintf(timeStamp, "%04d-%02d-%02d %02d:%02d:%02d",
          now.year(), now.month(), now.day(),
          now.hour(), now.minute(), now.second());

  // Populate JSON data
  data["device_id"] = DEVICE_ID;
  data["timestamp"] = timeStamp;
  data["wind_m_s"] = windSpeedMs;
  data["wind_kmh"] = windSpeedKmPerHour;
  data["rainrate_mm_h"] = rainRateMmPerHour;
  data["temperature_C"] = temperature;
  data["humidity_%"] = humidity;
  data["light_lux"] = lux;
  data["sol_voltage_V"] = solBusVoltage;
  data["sol_current_mA"] = solCurrent_mA;
  data["sol_power_W"] = solPower_mW / 1000.0;
  // data["bat_voltage_V"] = batBusVoltage;
  // data["bat_current_mA"] = batCurrent_mA;
  // data["bat_power_W"] = batPower_mW / 1000.0;

  String payload;
  serializeJson(data, payload);
  startHTTPPost(payload);
}

// ===== MAIN SETUP =====
void setup() {
  Serial.begin(115200); delay(2000);
  Serial.println("Weather Station Starting...");

  // Initialize peripherals
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.print("Initializing...");

  // Initialize sensors
  dht.begin();
  lightSensor.begin(BH1750::CONTINUOUS_HIGH_RES_MODE);
  ina219Solar.begin();
  // ina219Battery.begin();

  // Setup rain sensor interrupt
  pinMode(RAIN_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(RAIN_PIN), onTip, FALLING);

  // Setup wind sensor interrupt
  pinMode(ANEMOMETER_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ANEMOMETER_PIN), anemometerISR, RISING);

  // Initialize SIM7600
  simSerial.begin(115200, SERIAL_8N1, ESP_RX, ESP_TX); delay(2000);
  initModem();
  initHTTP();

  // Initialize RTC
  if (!rtc.begin()) {
    Serial.println("ERROR: RTC not detected!");
    lcd.clear();
    lcd.print("RTC ERROR!");
    while (1);
  }

  // Clear LCD and show ready status
  lcd.clear();
  lcd.print("System Ready");
  delay(2000);
  
  Serial.println("Setup completed successfully");
  lastPostTime = millis();
}

// ===== MAIN LOOP =====
void loop() {
  unsigned long now = millis();
  
  // Check HTTP POST status
  checkPostStatus();

  // Update fast sensors (wind, light)
  if (now - lastWindUpdateTime >= windUpdateInterval) {
    updateFastSensors();
    lastWindUpdateTime = now;
  }

  // Update slow sensors (temp, humidity, power)
  if (now - lastSlowSensorUpdateTime >= slowSensorUpdateInterval) {
    updateSlowSensors();
    lastSlowSensorUpdateTime = now;
  }

  // Update LCD display
  updateLCD();

  // Switch LCD pages
  if (now - lastPageSwitchTime >= pageSwitchInterval) {
    lcdPage = (lcdPage + 1) % 3;  // Cycle through 0-2
    lcd.clear(); updateLCD();
    lastPageSwitchTime = now;
  }

  // Send data to server
  if ((now - lastPostTime >= POST_INTERVAL) && !isPosting) {
    lastPostTime = now;
    sendSensorData();
  }
}