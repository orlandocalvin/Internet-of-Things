#include <WiFi.h>
#include <HTTPClient.h>
#include <HCSR04.h>

HCSR04 hc(2,4); // pin 2 trigger, pin 4 echo
#include "secrets.h"  // credentials (gitignored; see secrets.example.h)

void setup() {
  delay(1000);
  Serial.begin(115200);
  WiFi.mode(WIFI_OFF);
  delay(1000);
  WiFi.mode(WIFI_STA);

  WiFi.begin(ssid, password);
  Serial.println("");

  Serial.print("Connecting");
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  //IP address assigned to your ESP
}

void loop() {
  
  int jarak = hc.dist();
  String postData = (String)"jarak=" + jarak;

  HTTPClient http;
  http.begin("https://orlandocalvin.000webhostapp.com/jarak/api_jarak.php");
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");

  auto httpCode = http.POST(postData);
  String payload = http.getString();

  Serial.println(postData);
  Serial.println(payload);

  http.end();
  delay(1000);
}


















