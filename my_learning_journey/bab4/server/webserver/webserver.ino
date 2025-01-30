#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "REDACTED";
const char* password = "REDACTED";

const int led = 2;

// Membuat IP Static
// IPAddress staticIP(192,168,1,2);
// IPAddress gateway(192,168,1,1);
// IPAddress subnet(255,255,255,0);

WebServer server(80);

void setup() {
  pinMode(led,OUTPUT);

  // Membuat Acces Point
  // WiFi.softAPConfig(staticIP, gateway, subnet);
  // WiFi.softAP(ssid, password);

  // Terhubung Ke Wifi
  // WiFi.config(staticIP, gateway, subnet);
  WiFi.begin(ssid, password);

  server.on("/", handleRoot);
  server.on("/on", handleOn);
  server.on("/off", handleOff);

  server.begin();
}

void loop() {
  server.handleClient();
}

void handleRoot() {
  server.send(200, "text/html", SendHTML());
} void handleOn() {
  digitalWrite(led, HIGH);
  server.send(200, "text/html", SendHTML());
} void handleOff() {
  digitalWrite(led, LOW);
  server.send(200, "text/html", SendHTML());
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