#include <WiFi.h>
#include <ESPAsyncWebServer.h>

#include "secrets.h"  // credentials (gitignored; see secrets.example.h)

const int led = 2;

AsyncWebServer server(80);

void setup() {
  pinMode(led, OUTPUT);

  WiFi.begin(ssid, password);

  server.on("/", [](AsyncWebServerRequest *request) {
    request->send(200, "text/html", SendHTML());
  });
  server.on("/on", [](AsyncWebServerRequest *request) {
    digitalWrite(led, HIGH);
    request->send(200, "text/html", SendHTML());
  });
  server.on("/off", [](AsyncWebServerRequest *request) {
    digitalWrite(led, LOW);
    request->send(200, "text/html", SendHTML());
  });

  server.begin();
}

void loop() {
  // Kosong??
}

String SendHTML() {
  return R"(
    <html>
      <body>
        <h1>ESP32 LED Control</h1>
        <p>
          <a href="/on"><button>ON</button></a>
          <a href="/off"><button>OFF</button></a>
        </p>
      </body>
    </html>
  )";
}