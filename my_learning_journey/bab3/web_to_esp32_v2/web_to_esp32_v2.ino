#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>

const char* ssid = "esp32";
const char* password = "12341234";

const int LED_BUILTIN = 2;
bool ledState = LOW;

AsyncWebServer server(80);

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, ledState);

  WiFi.begin(ssid, password);
  SPIFFS.begin();

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/index.html", "text/html");
  });

  server.on("/toggle", HTTP_GET, [](AsyncWebServerRequest *request) {
    ledState = !ledState;
    digitalWrite(LED_BUILTIN, ledState);
    request->send(200, "text/plain", "");
  });

  server.begin();
}

void loop() {
  // Kosong?
}
