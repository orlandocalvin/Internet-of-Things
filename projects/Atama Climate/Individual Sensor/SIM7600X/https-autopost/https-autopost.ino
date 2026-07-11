#include <DHT.h>
#include <ArduinoJson.h>
#include <HardwareSerial.h>

// Pin definitions
#define DHT_PIN 4
#define DHT_TYPE DHT22
#define ESP_RX 16
#define ESP_TX 17

// Network configuration
const char* APN = "indosatgprs";
const char* SERVER_URL = "https://httpbin.org/post";

// Timing configuration
const unsigned long POST_INTERVAL = 10000; // 10 seconds

// Hardware objects
HardwareSerial simSerial(1);
DHT dht(DHT_PIN, DHT_TYPE);

// Global variables
unsigned long lastPostTime = 0;
unsigned long postStartTime = 0;
bool isPosting = false;

void initModem() {
  sendATCommand("AT"); delay(500);
  sendATCommand("AT+CPIN?"); delay(500);
  sendATCommand("AT+CSQ"); delay(500);
  sendATCommand("AT+CREG?"); delay(500);

  sendATCommand("AT+CGDCONT=1,\"IP\",\"" + String(APN) + "\""); delay(1000); // Set APN
  sendATCommand("AT+CGATT=1"); delay(2000); // Attach to network
  sendATCommand("AT+CGACT=1,1"); delay(4000); // Activate PDP context
  sendATCommand("AT+CGPADDR=1"); delay(1000); // Get IP address

  clearBuffer();
}

void initHTTP() {
  sendATCommand("AT+HTTPTERM"); delay(500); // Terminate existing session
  sendATCommand("AT+HTTPINIT"); delay(1000); // Initialize HTTP service

  // Set parameters
  sendATCommand("AT+HTTPPARA=\"CID\",1"); delay(200);
  sendATCommand("AT+HTTPPARA=\"URL\",\"" + String(SERVER_URL) + "\""); delay(200);
  sendATCommand("AT+HTTPPARA=\"CONTENT\",\"application/json\""); delay(200);

  clearBuffer();
}

void sendSensorData() {
  // Read sensor values
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  // Validate sensor readings
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("ERROR: Failed to read DHT22 sensor");
    return;
  }

  // Create JSON payload
  StaticJsonDocument<128> data;
  data["sensor"] = "DHT22";
  data["temperature"] = round(temperature * 10) / 10.0; // Round to 1 decimal
  data["humidity"] = round(humidity * 10) / 10.0;

  String payload;
  serializeJson(data, payload);

  // Start HTTP POST
  startHTTPPost(payload);
}

void startHTTPPost(String payload) {
  if (isPosting) return; // Prevent overlapping requests

  Serial.println("\n=== SENDING DATA ===");
  Serial.println("Payload: " + payload);

  // Set data length and timeout
  sendATCommand("AT+HTTPDATA=" + String(payload.length()) + ",10000");
  delay(300);

  simSerial.print(payload); delay(500); // Send payload data
  sendATCommand("AT+HTTPACTION=1"); // Execute POST request

  // Set POST state
  isPosting = true;
  postStartTime = millis();
}

void checkPostStatus() {
  if (!isPosting) return;
  
  // Check for timeout
  if (millis() - postStartTime > 15000) {
    Serial.println("POST TIMEOUT - Request failed");
    Serial.println("==================");
    isPosting = false;
    return;
  }
  
  // Check for response
  String response = "";
  while (simSerial.available()) {
    response += char(simSerial.read());
  }
  
  // Parse HTTP response
  if (response.indexOf("+HTTPACTION:") != -1) {
    if (response.indexOf(",200,") != -1) {
      Serial.println("POST SUCCESS - Data sent successfully");
    } else {
      Serial.println("POST FAILED - Server error");
      Serial.println("Response: " + response);
    }
    
    Serial.println("==================");
    
    // Reset POST state
    isPosting = false;
    
    // Optional: Read response body
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
  
  dht.begin(); // Initialize sensors
  initModem(); // Initialize modem
  initHTTP(); // Initialize HTTP session
  
  Serial.println("Setup Completed");
  lastPostTime = millis();
}

void loop() {
  unsigned long currentTime = millis();
  checkPostStatus(); // Check ongoing POST status
  
  // Send data at specified intervals
  if ((currentTime - lastPostTime >= POST_INTERVAL) && !isPosting) {
    lastPostTime = currentTime;
    sendSensorData();
  }
  
  delay(50); // Prevent excessive CPU usage
}