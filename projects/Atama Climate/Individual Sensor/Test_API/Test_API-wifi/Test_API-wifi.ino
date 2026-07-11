#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// WiFi credentials
const char* ssid = "enumatechz";
const char* password = "3numaTechn0l0gy";

// API endpoint
const char* serverURL = "https://www.atamagri.app/api/iot/sensor-data";

// Device configuration
const String deviceID = "ESP32-001";

void sendSensorData(float temp, float hum, float soil, float ph, float n, float p, float k) {
  HTTPClient http;
  http.begin(serverURL); // default port 443 for HTTPS
  http.addHeader("Content-Type", "application/json");

  // JSON payload
  DynamicJsonDocument data(512);
  data["device_id"] = deviceID;
  data["temperature"] = temp;
  data["humidity"] = hum;
  data["soil_moisture"] = soil;
  data["ph"] = ph;
  data["nitrogen"] = n;
  data["phosphorus"] = p;
  data["potassium"] = k;

  String jsonString;
  serializeJson(data, jsonString);

  // POST request
  int httpResponseCode = http.POST(jsonString);

  if (httpResponseCode > 0) {
    Serial.println("HTTP Response code: " + String(httpResponseCode));
    Serial.println("Response body: " + http.getString());
  } else {
    Serial.println("Error sending POST: " + String(httpResponseCode));
  } Serial.println();

  http.end();
}

// Dummy sensor functions
float readTemperature()   { return random(200, 350) / 10.0; } // 20-35°C
float readHumidity()      { return random(400, 800) / 10.0; } // 40-80%
float readSoilMoisture()  { return random(300, 700) / 10.0; } // 30-70%
float readPH()            { return random(60, 80) / 10.0; }   // 6.0-8.0
float readNitrogen()      { return random(10, 50); }          // 10-50 ppm
float readPhosphorus()    { return random(5, 25); }           // 5-25 ppm
float readPotassium()     { return random(15, 60); }          // 15-60 ppm

void setup() {
  Serial.begin(115200);

  // Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    // Dummy sensor readings
    float temperature = readTemperature();
    float humidity = readHumidity();
    float soilMoisture = readSoilMoisture();
    float ph = readPH();
    float nitrogen = readNitrogen();
    float phosphorus = readPhosphorus();
    float potassium = readPotassium();

    // Send to server
    sendSensorData(temperature, humidity, soilMoisture, ph, nitrogen, phosphorus, potassium);
  } else {
    Serial.println("WiFi disconnected, retrying...");
    WiFi.reconnect();
  }

  delay(30000); // every 30 seconds
}