#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "REDACTED"
#define BLYNK_TEMPLATE_NAME "REDACTED"
#define BLYNK_AUTH_TOKEN "REDACTED"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

const char* ssid = "REDACTED";
const char* password = "REDACTED";

void setup() {
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, password);
}

void loop() {
  Blynk.run();
}