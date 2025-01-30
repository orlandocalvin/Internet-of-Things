#include <WiFi.h>
#include <WebServer.h>
#include <SPIFFS.h>

const char* ssid = "REDACTED";
const char* password = "REDACTED";

const int led = 2;

WebServer server(80);

void setup() {
  pinMode(led, OUTPUT);
  
  SPIFFS.begin();
  WiFi.begin(ssid, password);

  server.on("/", handleRoot);
  server.on("/style.css", styleCss);
  server.on("/script.js", scriptJs);
  server.on("/on", handleOn);
  server.on("/off", handleOff);

  server.begin();
}

void loop() {
  server.handleClient();
}

void handleRoot() {
  server.send(200, "text/html", loadFromSPIFFS("/index.html"));
} void styleCss() {
  server.send(200, "text/css", loadFromSPIFFS("/style.css"));
} void scriptJs() {
  server.send(200, "text/js", loadFromSPIFFS("/script.js"));
} void handleOn() {
  digitalWrite(led, HIGH);
  server.send(200, "text/html", loadFromSPIFFS("/index.html"));
} void handleOff() {
  digitalWrite(led, LOW);
  server.send(200, "text/html", loadFromSPIFFS("/index.html"));
}

String loadFromSPIFFS(String path) {
  String content = "";
  File file = SPIFFS.open(path, "r");

  while (file.available()) {
    content += (char)file.read();
  }
  
  file.close();
  return content;
}
