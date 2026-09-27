#include <WiFi.h>
#include <ESPAsyncWebServer.h>

#include "secrets.h"  // credentials (gitignored; see secrets.example.h)
bool ledState = LOW;

AsyncWebServer server(80);

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, ledState);

  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    String html = R"(
    <!DOCTYPE html>
    <html>
    <head>
        <title>Kontrol LED ESP32</title>
        <link rel="stylesheet" href="https://maxcdn.bootstrapcdn.com/bootstrap/4.5.2/css/bootstrap.min.css">
        <style>
            body {
                background-color: #f0f0f0;
                text-align: center;
            }
            .container {
                margin-top: 20px;
                background-color: #ffffff;
                padding: 20px;
                border-radius: 10px;
                box-shadow: 0px 0px 10px #888888;
            }
            h1 {
                color: #007bff;
            }
        </style>
    </head>
    <body>
        <div class="container">
            <h1 class="mt-4">Kontrol LED ESP32</h1>
            <div class="custom-control custom-switch">
                <input type="checkbox" class="custom-control-input" id="controlSwitch" name="control" value="on">
                <label class="custom-control-label" for="controlSwitch">Hidupkan LED</label>
            </div>
        </div>
        <script src="https://code.jquery.com/jquery-3.5.1.slim.min.js"></script>
        <script src="https://cdn.jsdelivr.net/npm/@popperjs/core@2.11.6/dist/umd/popper.min.js"></script>
        <script src="https://maxcdn.bootstrapcdn.com/bootstrap/4.5.2/js/bootstrap.min.js"></script>
        <script>
            const controlSwitch = document.getElementById("controlSwitch");
            controlSwitch.addEventListener("change", () => {
                const newUrl = controlSwitch.checked ? "/toggle?control=on" : "/toggle?control=off";
                fetch(newUrl, { method: 'GET' });
            });
        </script>
    </body>
    </html>
  )";

    request->send(200, "text/html", html);
  });

  server.on("/toggle", HTTP_GET, [](AsyncWebServerRequest *request) {
    ledState = !ledState;
    digitalWrite(LED_BUILTIN, ledState);
    request->send(200, "text/plain", "OK");
  });

  server.begin();
}

void loop() {
  // Tidak ada operasi yang perlu dilakukan dalam loop
}
