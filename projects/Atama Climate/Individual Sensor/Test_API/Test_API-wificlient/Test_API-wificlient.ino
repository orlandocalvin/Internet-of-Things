#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>

// WiFi credentials
const char* ssid = "SUPER-ORCA";
const char* password = "zxcvbnmv";

// Server info
const char* host = "www.atamagri.app";
const int httpsPort = 443;
const char* endpoint = "/api/iot/sensor-data";

// Device configuration
const String deviceID = "ESP32-001";

void sendSensorData(float temp, float hum, float soil, float ph, float n, float p, float k) {
  WiFiClientSecure client;
  client.setInsecure(); // skip certificate validation

  Serial.print("Connecting to ");
  Serial.println(host);

  if (!client.connect(host, httpsPort)) {
    Serial.println("Connection failed!");
    return;
  }

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

  // Build HTTP request
  String request = String("POST ") + endpoint + " HTTP/1.1\r\n" +
                   "Host: " + host + "\r\n" +
                   "Content-Type: application/json\r\n" +
                   "Content-Length: " + jsonString.length() + "\r\n" +
                   "Connection: close\r\n\r\n" +
                   jsonString + "\r\n";

  client.print(request);

  // Print full response
  Serial.println("\n=== SERVER RESPONSE ===");
  while (client.connected() || client.available()) {
    if (client.available()) {
      String line = client.readStringUntil('\n');
      Serial.println(line);
    }
  }
  Serial.println("=== END RESPONSE ===");
  client.stop();
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
    float temperature = readTemperature();
    float humidity = readHumidity();
    float soilMoisture = readSoilMoisture();
    float ph = readPH();
    float nitrogen = readNitrogen();
    float phosphorus = readPhosphorus();
    float potassium = readPotassium();

    sendSensorData(temperature, humidity, soilMoisture, ph, nitrogen, phosphorus, potassium);
  } else {
    Serial.println("WiFi disconnected, retrying...");
    WiFi.reconnect();
  }

  delay(30000); // every 30 seconds
}