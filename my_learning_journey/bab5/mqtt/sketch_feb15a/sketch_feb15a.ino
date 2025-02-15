#define LED_PIN 2
#define BUTTON_PIN 15

bool ledState = LOW, lastButtonState = HIGH;

void setup() {
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  Serial.begin(115200);
}

void loop() {
  bool buttonState = digitalRead(BUTTON_PIN);

  if (buttonState == LOW && lastButtonState == HIGH) {
    digitalWrite(LED_PIN, ledState = !ledState);
    Serial.println(ledState ? "LED ON" : "LED OFF");
    delay(300);
  }

  lastButtonState = buttonState;
}