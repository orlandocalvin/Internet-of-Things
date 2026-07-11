#include <DHT.h>
#include <Wire.h>
#include <RTClib.h>
#include <BH1750.h>
#include <ArduinoJson.h>
#include <Adafruit_INA219.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <HTTPClient.h>

// Network config
const char* WIFI_SSID = "enumatechz";
const char* WIFI_PASSWORD = "3numaTechn0l0gy";
const char* SERVER_URL = "https://www.atamagri.app/api/iot/sensor-data";
const String DEVICE_ID = "ESP32-001";

// Pin definitions
const uint8_t DHT_PIN = 27;
const uint8_t RAIN_PIN = 14;
const uint8_t ANEMOMETER_PIN = 15;

// Sensor constants
const float MM_PER_TIP = 0.2f;
const float KMH_TO_MS = 0.27778f;
const float ANEMOMETER_FACTOR = 2.4f;
const uint32_t DEBOUNCE_TIME_MICROS = 5000UL;
const uint32_t RAIN_DEBOUNCE_MS = 250UL;

// Timing intervals (ms)
const uint32_t WIND_UPDATE_INTERVAL = 1500UL;
const uint32_t SLOW_SENSOR_INTERVAL = 5000UL;
const uint32_t PAGE_SWITCH_INTERVAL = 7000UL;
const uint32_t POST_INTERVAL = 10000UL;

// LCD config
const uint8_t LCD_ADDRESS = 0x27;
const uint8_t LCD_COLS = 20;
const uint8_t LCD_ROWS = 4;
const uint8_t TOTAL_PAGES = 3;

// Sensor objects
RTC_DS3231 rtc;
DHT dht(DHT_PIN, DHT22);
BH1750 lightSensor;
Adafruit_INA219 ina219Solar(0x40);
LiquidCrystal_I2C lcd(LCD_ADDRESS, LCD_COLS, LCD_ROWS);

// Sensor data structure
struct SensorData {
  float temperature = 0.0f;
  float humidity = 0.0f;
  float lux = 0.0f;
  float windSpeedMs = 0.0f;
  float windSpeedKmh = 0.0f;
  float rainRateMmh = 0.0f;
  float solarVoltage = 0.0f;
  float solarCurrentMa = 0.0f;
  float solarPowerMw = 0.0f;
};

// Volatile vars for interrupts
volatile uint32_t tipCount = 0;
volatile uint32_t lastTipTime = 0;
volatile uint32_t pulseCount = 0;
volatile uint32_t lastPulseTime = 0;

// Global variables
SensorData sensors;
uint8_t currentPage = 0;
uint32_t lastRainTipCount = 0;

// Timing variables
uint32_t lastWindUpdate = 0;
uint32_t lastSlowSensorUpdate = 0;
uint32_t lastPageSwitch = 0;
uint32_t lastPost = 0;

// Interrupt service routines
void IRAM_ATTR rainTipISR() {
  uint32_t currentTime = millis();
  if (currentTime - lastTipTime > RAIN_DEBOUNCE_MS) {
    tipCount++;
    lastTipTime = currentTime;
  }
}

void IRAM_ATTR anemometerISR() {
  uint32_t currentMicros = micros();
  if (currentMicros - lastPulseTime >= DEBOUNCE_TIME_MICROS) {
    pulseCount++;
    lastPulseTime = currentMicros;
  }
}

// WiFi connection
bool connectWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi");
  
  uint8_t attempts = 0;
  const uint8_t MAX_ATTEMPTS = 20;
  
  while (WiFi.status() != WL_CONNECTED && attempts < MAX_ATTEMPTS) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println(" Connected!");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
    return true;
  } else {
    Serial.println(" Failed!");
    return false;
  }
}

// Initialize sensors
bool initializeSensors() {
  // DHT22
  dht.begin();
  
  // Light sensor
  if (!lightSensor.begin(BH1750::CONTINUOUS_HIGH_RES_MODE)) {
    Serial.println("Light sensor init failed");
    return false;
  }
  
  // Solar panel monitor
  if (!ina219Solar.begin()) {
    Serial.println("INA219 init failed");
    return false;
  }
  
  // RTC
  if (!rtc.begin()) {
    Serial.println("RTC init failed");
    return false;
  }
  
  return true;
}

// Setup interrupts
void setupInterrupts() {
  pinMode(RAIN_PIN, INPUT_PULLUP);
  pinMode(ANEMOMETER_PIN, INPUT_PULLUP);
  
  attachInterrupt(digitalPinToInterrupt(RAIN_PIN), rainTipISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(ANEMOMETER_PIN), anemometerISR, RISING);
}

// Update wind and light sensors (fast)
void updateFastSensors() {
  // Wind speed calculation
  detachInterrupt(digitalPinToInterrupt(ANEMOMETER_PIN));
  uint32_t pulses = pulseCount;
  pulseCount = 0;
  attachInterrupt(digitalPinToInterrupt(ANEMOMETER_PIN), anemometerISR, RISING);
  
  uint32_t currentTime = millis();
  float elapsedSeconds = (currentTime - lastWindUpdate) / 1000.0f;
  
  if (elapsedSeconds > 0) {
    sensors.windSpeedKmh = (pulses / elapsedSeconds) * ANEMOMETER_FACTOR;
    sensors.windSpeedMs = sensors.windSpeedKmh * KMH_TO_MS;
  } else {
    sensors.windSpeedKmh = 0;
    sensors.windSpeedMs = 0;
  }
  
  // Light sensor
  float lux = lightSensor.readLightLevel();
  sensors.lux = (lux >= 0) ? lux : 0;
}

// Update temperature, humidity, rain, solar (slow)
void updateSlowSensors() {
  // Rain rate calculation
  uint32_t newTips = tipCount - lastRainTipCount;
  lastRainTipCount = tipCount;
  sensors.rainRateMmh = newTips * MM_PER_TIP * 720.0f; // Convert to mm/hour
  
  // Temperature and humidity
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();
  
  if (!isnan(temp) && !isnan(hum)) {
    sensors.temperature = temp;
    sensors.humidity = hum;
  }
  
  // Solar panel data
  sensors.solarVoltage = ina219Solar.getBusVoltage_V();
  sensors.solarCurrentMa = ina219Solar.getCurrent_mA();
  sensors.solarPowerMw = ina219Solar.getPower_mW();
}

// LCD helper functions
void lcdPrintAt(uint8_t col, uint8_t row, const String& text, uint8_t width = 0) {
  lcd.setCursor(col, row);
  lcd.print(text);
  
  // Clear remaining chars if width specified
  if (width > 0) {
    for (uint8_t i = text.length(); i < width; i++) {
      lcd.print(" ");
    }
  }
}

void displayPage0() { // Wind & Rain
  lcdPrintAt(0, 0, "=== WIND & RAIN ===", LCD_COLS);
  lcdPrintAt(0, 1, "Wind: " + String(sensors.windSpeedMs, 1) + " m/s", LCD_COLS);
  lcdPrintAt(0, 2, "Wind: " + String(sensors.windSpeedKmh, 1) + " km/h", LCD_COLS);
  lcdPrintAt(0, 3, "Rain: " + String(sensors.rainRateMmh, 1) + " mm/h", LCD_COLS);
}

void displayPage1() { // Weather
  lcdPrintAt(0, 0, "==== WEATHER ====", LCD_COLS);
  lcdPrintAt(0, 1, "Temp : " + String(sensors.temperature, 1) + " C", LCD_COLS);
  lcdPrintAt(0, 2, "Humid: " + String(sensors.humidity, 1) + " %", LCD_COLS);
  lcdPrintAt(0, 3, "Light: " + String((int)sensors.lux) + " lux", LCD_COLS);
}

void displayPage2() { // Solar
  lcdPrintAt(0, 0, "=== SOLAR PANEL ===", LCD_COLS);
  lcdPrintAt(0, 1, "Voltage: " + String(sensors.solarVoltage, 2) + " V", LCD_COLS);
  lcdPrintAt(0, 2, "Current: " + String(sensors.solarCurrentMa, 0) + " mA", LCD_COLS);
  lcdPrintAt(0, 3, "Power  : " + String(sensors.solarPowerMw/1000.0f, 2) + " W", LCD_COLS);
}

void updateLCDDisplay() {
  switch (currentPage) {
    case 0: displayPage0(); break;
    case 1: displayPage1(); break;
    case 2: displayPage2(); break;
    default: currentPage = 0; displayPage0(); break;
  }
}

// Create JSON payload
String createJsonPayload() {
    ;
  
  DateTime now = rtc.now();
  char timestamp[25];
  sprintf(timestamp, "%04d-%02d-%02d %02d:%02d:%02d",
          now.year(), now.month(), now.day(),
          now.hour(), now.minute(), now.second());
  
  doc["device_id"] = DEVICE_ID;
  doc["timestamp"] = timestamp;
  doc["wind_m_s"] = sensors.windSpeedMs;
  doc["wind_kmh"] = sensors.windSpeedKmh;
  doc["rainrate_mm_h"] = sensors.rainRateMmh;
  doc["temperature_C"] = sensors.temperature;
  doc["humidity_%"] = sensors.humidity;
  doc["light_lux"] = sensors.lux;
  doc["sol_voltage_V"] = sensors.solarVoltage;
  doc["sol_current_mA"] = sensors.solarCurrentMa;
  doc["sol_power_W"] = sensors.solarPowerMw / 1000.0f;
  
  String payload;
  serializeJson(doc, payload);
  return payload;
}

// Send data to server
void sendSensorData() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi disconnected, skipping POST");
    return;
  }
  
  HTTPClient http;
  http.begin(SERVER_URL);
  http.addHeader("Content-Type", "application/json");
  http.setTimeout(5000); // 5 second timeout
  
  String payload = createJsonPayload();
  int httpCode = http.POST(payload);
  
  if (httpCode > 0) {
    Serial.printf("\nPOST response: %d\n", httpCode);
    if (httpCode == HTTP_CODE_OK) {
      Serial.println("Data sent successfully");
    }
  } else {
    Serial.printf("POST failed: %s\n", http.errorToString(httpCode).c_str());
  }
  
  http.end();
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Weather Station Starting...");
  
  // Initialize LCD
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcdPrintAt(0, 0, "Initializing...");
  
  // Initialize sensors
  if (!initializeSensors()) {
    lcdPrintAt(0, 1, "Sensor Init Failed!");
    Serial.println("Sensor initialization failed");
    while (1) delay(1000); // Halt on failure
  }
  
  // Setup interrupts
  setupInterrupts();
  
  // Connect WiFi
  lcdPrintAt(0, 1, "Connecting WiFi...");
  if (!connectWiFi()) {
    lcdPrintAt(0, 2, "WiFi Failed!");
    // Continue without WiFi
  }
  
  // System ready
  lcd.clear();
  lcdPrintAt(0, 0, "System Ready");
  delay(2000);
  lcd.clear();
  
  // Initialize timing
  uint32_t now = millis();
  lastWindUpdate = now;
  lastSlowSensorUpdate = now;
  lastPageSwitch = now;
  lastPost = now;
  
  Serial.println("Setup complete");
}

void loop() {
  uint32_t currentTime = millis();
  
  // Update fast sensors (wind, light)
  if (currentTime - lastWindUpdate >= WIND_UPDATE_INTERVAL) {
    updateFastSensors();
    lastWindUpdate = currentTime;
  }
  
  // Update slow sensors (temp, humidity, rain, solar)
  if (currentTime - lastSlowSensorUpdate >= SLOW_SENSOR_INTERVAL) {
    updateSlowSensors();
    lastSlowSensorUpdate = currentTime;
  }
  
  // Update LCD display
  updateLCDDisplay();
  
  // Switch LCD page
  if (currentTime - lastPageSwitch >= PAGE_SWITCH_INTERVAL) {
    currentPage = (currentPage + 1) % TOTAL_PAGES;
    lcd.clear();
    lastPageSwitch = currentTime;
  }
  
  // Send data to server
  if (currentTime - lastPost >= POST_INTERVAL) {
    sendSensorData();
    lastPost = currentTime;
  }
  
  // Small delay to prevent watchdog issues
  delay(10);
}