const int ledM = 15, ledB = 2, ledK = 4, ledH = 5;
const int led[5] = {ledM,ledB,ledK,ledB,ledH};
bool ledState = LOW;

void setup() {
  for(int i=0;i<5;i++) {
    pinMode(led[i], OUTPUT);
  }
}

void loop() {
  for(int i=0;i<5;i++) {
    for(int j=0;j<6;j++) {
      ledState = !ledState;
      digitalWrite(led[i], ledState);
      delay(200);
    }
  }
}