#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>

#include "secrets.h"  // credentials (gitignored; see secrets.example.h)

// membuat static IP Address
IPAddress staticIP(192,168,1,2);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);

int led[] = {2, 4, 5};
bool state[] = {LOW,LOW,LOW};

AsyncWebServer server(80);

void setup() {
  for(int i=0;i<3;i++) {
    pinMode(led[i], OUTPUT);
  }
  // Membuat Acces Point
  WiFi.softAPConfig(staticIP, gateway, subnet);
  WiFi.softAP(ssid, password);

  // Terhubung Ke Wifi
  // WiFi.config(staticIP, gateway, subnet);
  // WiFi.begin(ssid, password);

  SPIFFS.begin();

  server.on("/", [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/index.html", "text/html");
  });
  server.on("/style.css", [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/style.css", "text/css");
  });
  server.on("/script.js", [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/script.js", "text/js");
  });

  serverOn("/toggle1", 0);
  serverOn("/toggle2", 1);
  serverOn("/toggle3", 2);

  server.begin();
}

void loop() {
  // Kosong??
}

void serverOn(char* toggle, int i) {
  server.on(toggle, [i](AsyncWebServerRequest *request) {
    state[i] = !state[i];
    digitalWrite(led[i],state[i]);
    request->send(200, "text/plain", "");
  });
}