#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

const char* ssid = "REDACTED";
const char* password = "REDACTED";
const int udpPort = 1234;

WiFiUDP udp;
char incomingPacket[255];

void setup() {
  Serial.begin(74880);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("\nConnected to WiFi");

  udp.begin(udpPort);
}

void loop() {
  int packetSize = udp.parsePacket();
  if (packetSize) {
    int len = udp.read(incomingPacket, sizeof(incomingPacket) - 1);
    if (len > 0) {
      incomingPacket[len] = '\0'; // Null-Terminator
    }
    Serial.print("Received: ");
    Serial.println(incomingPacket);
  }
}