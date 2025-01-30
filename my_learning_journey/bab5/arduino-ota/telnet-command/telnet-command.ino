#include <WiFi.h>
#include <ESPmDNS.h>
#include <ArduinoOTA.h>

const int LED_PIN = 4;
String inputCommand = "";

const char *ssid = "REDACTED";
const char *password = "REDACTED";

IPAddress local_IP(192, 168, 137, 100);
IPAddress gateway(192, 168, 137, 1);
IPAddress subnet(255, 255, 255, 0);

// Telnet Server
WiFiServer telnetServer(23);
WiFiClient telnetClient;

void setup() {
  pinMode(LED_PIN, OUTPUT);

  Serial.begin(115200);

  // Setup WiFi with static IP
  WiFi.config(local_IP, gateway, subnet);
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

      telnetClient.println("=== Telnet Connected ===");
      telnetClient.println("Type 'LED_ON' to turn ON the LED.");
      telnetClient.println("Type 'LED_OFF' to turn OFF the LED.");
      telnetClient.println("=========================\n");
    }
  }

  // Read data from client
  if (telnetClient && telnetClient.connected()) {
    if (telnetClient.available()) {
      char incomingChar = telnetClient.read();

      if (incomingChar == '\n') {
        inputCommand.trim();  // Clean input
        inputCommand.toUpperCase();  // Normalize case

        // Command processing
        if (inputCommand == "LED_ON") {
          digitalWrite(LED_PIN, HIGH);
          telnetClient.println("LED is ON");
        } else if (inputCommand == "LED_OFF") {
          digitalWrite(LED_PIN, LOW);
          telnetClient.println("LED is OFF");
        } else {
          telnetClient.println("Invalid command. Use 'LED_ON' or 'LED_OFF'");
        }

        inputCommand = "";  // Reset input buffer
      } else {
        inputCommand += incomingChar;  // Add to input buffer
      }
    }
  }
}