#include <ArduinoJson.h>
#include <HardwareSerial.h>

// Pin definitions
#define ESP_RX 16
#define ESP_TX 17

// Network configuration
const char* APN = "indosatgprs";
const char* SERVER_URL = "https://www.atamagri.app/api/iot/sensor-data";

// Timing configuration
const unsigned long POST_INTERVAL = 10000; // 10 seconds

// Hardware objects
HardwareSerial simSerial(1);

// Global variables
unsigned long lastPostTime = 0;
unsigned long postStartTime = 0;
bool isPosting = false;

void initModem() {
  sendATCommand("AT"); delay(500);
  sendATCommand("AT+CPIN?"); delay(500);
  sendATCommand("AT+CSQ"); delay(500);
  sendATCommand("AT+CREG?"); delay(500);

  sendATCommand("AT+CGDCONT=1,\"IP\",\"" + String(APN) + "\""); delay(1000);
  sendATCommand("AT+CGATT=1"); delay(2000);
  sendATCommand("AT+CGACT=1,1"); delay(4000);
  sendATCommand("AT+CGPADDR=1"); delay(1000);

  clearBuffer();
}

void initHTTP() {
  sendATCommand("AT+HTTPTERM"); delay(500);
  sendATCommand("AT+HTTPINIT"); delay(1000);

  sendATCommand("AT+HTTPPARA=\"CID\",1"); delay(200);
  sendATCommand("AT+HTTPPARA=\"URL\",\"" + String(SERVER_URL) + "\""); delay(200);
  sendATCommand("AT+HTTPPARA=\"CONTENT\",\"application/json\""); delay(200);

  clearBuffer();
}

void sendSensorData() {
  // Dummy sensor values (randomized each send)
  String deviceId = "ESP32-001";
  
  // Timestamp sederhana (dummy, tanpa RTC/ NTP)
  String timestamp = "2025-09-16 12:45:33"; 
  
  // Generate dummy values
  float wind_ms       = 1.0 + random(0, 500) / 100.0;   // 1.0 – 6.0 m/s
  float wind_kmh      = wind_ms * 3.6;                  // konversi
  float rainrate      = random(0, 50) / 100.0;          // 0.0 – 0.5 mm/h
  float temperature   = 25 + random(0, 80) / 10.0;      // 25.0 – 32.9 °C
  float humidity      = 50 + random(0, 500) / 10.0;     // 50.0 – 99.9 %
  float lightLux      = 100 + random(0, 2000) / 10.0;   // 100 – 300.0 lux
  float solVoltage    = 11 + random(0, 30) / 10.0;      // 11.0 – 13.9 V
  float solCurrent    = 100 + random(0, 1000) / 10.0;   // 100.0 – 200.0 mA
  float solPower      = (solVoltage * solCurrent) / 1000.0; // W

  // Create JSON payload
  StaticJsonDocument<256> data;
  data["device_id"]      = deviceId;
  data["timestamp"]      = timestamp;
  data["wind_m_s"]       = wind_ms;
  data["wind_kmh"]       = wind_kmh;
  data["rainrate_mm_h"]  = rainrate;
  data["temperature_C"]  = temperature;
  data["humidity_%"]     = humidity;
  data["light_lux"]      = lightLux;
  data["sol_voltage_V"]  = solVoltage;
  data["sol_current_mA"] = solCurrent;
  data["sol_power_W"]    = solPower;

  String payload;
  serializeJson(data, payload);

  // Start HTTP POST
  startHTTPPost(payload);
}

void startHTTPPost(String payload) {
  if (isPosting) return;

  Serial.println("\n=== SENDING DATA ===");
  Serial.println("Payload: " + payload);

  sendATCommand("AT+HTTPDATA=" + String(payload.length()) + ",10000");
  delay(300);

  simSerial.print(payload); delay(500);
  sendATCommand("AT+HTTPACTION=1");

  isPosting = true;
  postStartTime = millis();
}

void checkPostStatus() {
  if (!isPosting) return;
  
  if (millis() - postStartTime > 15000) {
    Serial.println("POST TIMEOUT - Request failed");
    Serial.println("==================");
    isPosting = false;
    return;
  }
  
  String response = "";
  while (simSerial.available()) {
    response += char(simSerial.read());
  }
  
  if (response.indexOf("+HTTPACTION:") != -1) {
    if (response.indexOf(",200,") != -1) {
      Serial.println("POST SUCCESS - Data sent successfully");
    } else {
      Serial.println("POST FAILED - Server error");
      Serial.println("Response: " + response);
    }
    
    Serial.println("==================");
    isPosting = false;

    sendATCommand("AT+HTTPREAD=0,200");
    delay(500);
    clearBuffer();
  }
}

void sendATCommand(String command) {
  simSerial.println(command);
}

void clearBuffer() {
  while (simSerial.available()) {
    simSerial.read();
  }
}

void setup() {
  Serial.begin(115200);
  simSerial.begin(115200, SERIAL_8N1, ESP_RX, ESP_TX);
  delay(2000);
  
  Serial.print("Setup Begin... ");
  
  initModem();
  initHTTP();
  
  Serial.println("Setup Completed");
  lastPostTime = millis();

  randomSeed(analogRead(0)); // Seed random generator
}

void loop() {
  unsigned long currentTime = millis();
  checkPostStatus();
  
  if ((currentTime - lastPostTime >= POST_INTERVAL) && !isPosting) {
    lastPostTime = currentTime;
    sendSensorData();
  }
  
  delay(50);
}