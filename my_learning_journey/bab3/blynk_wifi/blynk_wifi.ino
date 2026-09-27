#define BLYNK_PRINT Serial
#include "secrets.h"  // credentials (gitignored; see secrets.example.h)

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>


void setup() {
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, password);
}

void loop() {
  Blynk.run();
}