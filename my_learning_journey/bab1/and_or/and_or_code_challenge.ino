const int ledM = 12;
const int ledH = 11;
const int ledK = 10;
const int pbM = 5;
const int pbH = 6;

void setup() {
  pinMode(ledM, OUTPUT);
  pinMode(ledH, OUTPUT);
  pinMode(ledK, OUTPUT);
  pinMode(pbM, INPUT);
  pinMode(pbH, INPUT);
}

void loop() {
  if(digitalRead(pbM) == 0 && digitalRead(pbH) == 0) {
    digitalWrite(ledM, HIGH);
    digitalWrite(ledK, HIGH);
    digitalWrite(ledH, HIGH);
  } else if(digitalRead(pbM) == 1 && digitalRead(pbH) == 0) {
    digitalWrite(ledM, LOW);
    digitalWrite(ledK, HIGH);
    digitalWrite(ledH, HIGH);
  } else if(digitalRead(pbM) == 0 && digitalRead(pbH) == 1) {
    digitalWrite(ledM, HIGH);
    digitalWrite(ledK, HIGH);
    digitalWrite(ledH, LOW);
  } else if(digitalRead(pbM) == 1 && digitalRead(pbH) == 1) {
    digitalWrite(ledM, HIGH);
    digitalWrite(ledK, LOW);
    digitalWrite(ledH, HIGH);
  }
}