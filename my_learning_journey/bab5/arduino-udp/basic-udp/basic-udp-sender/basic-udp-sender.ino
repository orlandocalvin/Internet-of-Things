#include <WiFi.h>
#include <WiFiUdp.h>

#include "secrets.h"  // credentials (gitignored; see secrets.example.h)
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
  const char* message = "Hello from sender";
  
  udp.beginPacket(receiverIP, udpPort);
  udp.print(message);
  udp.endPacket();

  Serial.println("Message sent!");
  delay(1000);
}