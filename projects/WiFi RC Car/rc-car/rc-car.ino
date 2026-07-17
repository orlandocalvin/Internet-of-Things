#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>

// connections & values for drive Motors
const int RIGHT_GAS = D1, LEFT_GAS = D2, RIGHT_DIR = D3, LEFT_DIR = D4;
const int OFF = 0, ON = 1, FORWARD = 0, BACKWARD = 1;

const int buzPin = D5;
const int ledPin = D8;

String command;
AsyncWebServer server(80);

// set AP variable
const char* ssid = "ESP8266 RC CAR";
const char* password = "zxcvbnmv";
const int channel = 6, max_connections = 2;
bool hidden = false;

const int udpPort = 1234;
WiFiUDP udp;
char incomingPacket[256];

void setup(){
  Serial.begin(74880);
  
  // set buzzer & led
  pinMode(buzPin, OUTPUT);
  pinMode(ledPin, OUTPUT);
  digitalWrite(buzPin, LOW);
  digitalWrite(ledPin, LOW);
    
  // Set all the motor control pins to outputs
  pinMode(RIGHT_GAS, OUTPUT);
  pinMode(LEFT_GAS, OUTPUT);
  pinMode(RIGHT_DIR, OUTPUT);
  pinMode(LEFT_DIR, OUTPUT);
  
  // Turn off motors - Initial state
  digitalWrite(RIGHT_DIR, LOW);
  digitalWrite(LEFT_DIR, LOW);
  digitalWrite(RIGHT_GAS, 0);
  digitalWrite(LEFT_GAS, 0);

  // Setup WiFi AP
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password, channel, hidden, max_connections);
  Serial.println("AP IP address: "+ WiFi.softAPIP().toString());
  udp.begin(udpPort);

  // Mount LittleFS
  if (!LittleFS.begin()) {
    Serial.println("LittleFS mount failed");
    return;
  }

  // Configuring static pages from LittleFS
  server.serveStatic("/", LittleFS, "/").setDefaultFile("index.html");

  server.on("/command", HTTP_GET, [](AsyncWebServerRequest *request){
    if (request->hasParam("State")) {
      command = request->getParam("State")->value();

      if (command == "F") Forward();
      else if (command == "B") Backward();
      else if (command == "R") TurnRight();
      else if (command == "L") TurnLeft();
      else if (command == "S") Stop();
      else if (command == "V") BeepHorn();
      else if (command == "W") TurnLightOn();
      else if (command == "w") TurnLightOff();

      request->send(200, "text/plain", "OK");
    } else {
      request->send(400, "text/plain", "Missing parameter");
    }
  });

  server.begin();
}

void loop() {
  int packetSize = udp.parsePacket();
  if (packetSize) {
    int len = udp.read(incomingPacket, sizeof(incomingPacket) - 1);
    if (len > 0) {
      incomingPacket[len] = '\0'; // Null-Terminator
    }

    // Parsing JSON
    StaticJsonDocument<256> data;
    deserializeJson(data, incomingPacket);

    float roll = data["roll"];
    float pitch = data["pitch"];

    if (pitch > 30 && pitch < 100) Forward();
    else if (pitch < -30 && pitch > -100) Backward();
    else if (roll < -30 && roll > -80) TurnRight();
    else if (roll > 30 && roll < 80) TurnLeft();
    else Stop();
  }
}

void Throttle(int gas) {
  digitalWrite(RIGHT_GAS, gas);
  digitalWrite(LEFT_GAS, gas);
}

void Forward(){ digitalWrite(RIGHT_DIR, FORWARD); digitalWrite(LEFT_DIR, FORWARD); Throttle(ON); }
void Backward(){ digitalWrite(RIGHT_DIR, BACKWARD); digitalWrite(LEFT_DIR, BACKWARD); Throttle(ON); }
void TurnRight(){ digitalWrite(RIGHT_DIR, BACKWARD); digitalWrite(LEFT_DIR, FORWARD); Throttle(ON); }
void TurnLeft(){ digitalWrite(RIGHT_DIR, FORWARD); digitalWrite(LEFT_DIR, BACKWARD); Throttle(ON); }
void Stop(){ Throttle(OFF); }
void BeepHorn(){ digitalWrite(buzPin, HIGH); delay(150); digitalWrite(buzPin, LOW); }
void TurnLightOn(){ digitalWrite(ledPin, HIGH); }
void TurnLightOff(){ digitalWrite(ledPin, LOW); }