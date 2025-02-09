#include <WiFi.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>

const char* ssid = "REDACTED";
const char* password = "REDACTED";
const char* receiverIP = "192.168.137.44";
const int udpPort = 1234;

WiFiUDP udp;

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("\nConnected to WiFi");
}

void loop() {
  StaticJsonDocument<256> data;
  data["roll"] = 12.5;
  data["pitch"] = -3.7;
  
  // Sends JSON as String
  String jsonString;
  serializeJson(data, jsonString);
  
  // Send JSON via UDP
  udp.beginPacket(receiverIP, udpPort);
  udp.print(jsonString);
  udp.endPacket();

  Serial.println("Sent JSON: " + String(jsonString));

  delay(1000);
}