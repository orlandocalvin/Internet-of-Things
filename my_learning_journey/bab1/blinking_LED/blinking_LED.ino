const int pinLed = 3;
void setup() {
  pinMode(pinLed, OUTPUT);
}

void loop() {
  digitalWrite(pinLed, HIGH);
  delay(1000); // Wait for 1000 millisecond(s)
  digitalWrite(pinLed, LOW);
  delay(1000); // Wait for 1000 millisecond(s)
}