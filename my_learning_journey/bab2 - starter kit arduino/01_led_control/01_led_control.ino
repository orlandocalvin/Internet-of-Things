const int led[] = {5, 6, 7};

void ledRun(bool state) {
  for(int i=0; i<3; i++) {
    digitalWrite(led[i], state);
    delay(200);
  }
}

void setup() {
  for(int i=0; i<3; i++) {
    pinMode(led[i], OUTPUT);
  }
}

void loop() {
  ledRun(HIGH);
  ledRun(LOW);
}