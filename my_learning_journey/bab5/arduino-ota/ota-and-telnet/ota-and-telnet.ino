#include <WiFi.h>
#include <ESPmDNS.h>
#include <ArduinoOTA.h>

const int LED_PIN = 4;

#include "secrets.h"  // credentials (gitignored; see secrets.example.h)

// Telnet Server
WiFiServer telnetServer(23);
WiFiClient telnetClient;

void ledBlink() {
  digitalWrite(LED_PIN, HIGH);
  telnetClient.println("LED ON");
  delay(1000);

  digitalWrite(LED_PIN, LOW);
  telnetClient.println("LED OFF");
  delay(1000);
}

void setup() {
  pinMode(LED_PIN, OUTPUT);

  Serial.begin(115200);
  
  // Setup WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected!");

  // Start OTA
  ArduinoOTA.setHostname("esp32-OTA");
  ArduinoOTA.setPassword("admin");
  ArduinoOTA.begin();

  // Start Telnet Server
  telnetServer.begin();
  telnetServer.setNoDelay(true);
  
  Serial.println("Telnet Server started at: " + WiFi.localIP().toString());
}

void loop() {
  ArduinoOTA.handle();

  // Check telnet connection
  if (telnetServer.hasClient()) {
    if (!telnetClient || !telnetClient.connected()) {
      if (telnetClient) telnetClient.stop();
      telnetClient = telnetServer.available();

      telnetClient.println("=== Telnet Connected ===\n");
    }
  }

  // Led Blink with Telnet Log
  ledBlink();
}