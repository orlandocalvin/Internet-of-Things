unsigned long prevMillis = 0;
const int ledPin = 2;
bool ledState = LOW;

void setup() {
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, ledState);
}

void loop() {
  unsigned long curMillis = millis();

  if (ledState && (curMillis - prevMillis >= 2000)) {
    ledState = !ledState;
    digitalWrite(ledPin, ledState);
    prevMillis = curMillis;
  } else if (!ledState && (curMillis - prevMillis >= 500)) {
    ledState = !ledState;
    digitalWrite(ledPin, ledState);
    prevMillis = curMillis;
  }
}
