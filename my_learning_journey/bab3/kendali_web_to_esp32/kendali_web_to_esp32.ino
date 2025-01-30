#include <WiFi.h>
#include <HTTPClient.h>

const char *ssid = "REDACTED";
const char *password = "REDACTED";

void setup() {
  Serial.begin(9600);
  WiFi.begin(ssid, password);
  pinMode(2,OUTPUT);
}

void loop() {
  HTTPClient http;
  http.begin("https://orlandocalvin.000webhostapp.com/kendali/light.json");

  int httpCode = http.GET();
  if(httpCode > 0) {
    if(httpCode == HTTP_CODE_OK) {
      String json = http.getString();
      Serial.println(json);

      if(json == "{\"light\": \"on\"}") {
        digitalWrite(2, HIGH);
        Serial.println("Lampu Nyala\n");
      } else {
        digitalWrite(2, LOW);
        Serial.println("Lampu Mati\n");
      }
    }
  } else {
    Serial.println(http.errorToString(httpCode));
  }

  http.end();
}