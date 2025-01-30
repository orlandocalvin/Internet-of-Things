unsigned long previousMillis = 0;

void setup() {
  Serial.begin(9600);
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= 1000) {
    Serial.println(String(currentMillis) + " - " + String(previousMillis) + " = " + String(currentMillis - previousMillis));
    previousMillis = currentMillis;
  }
}
