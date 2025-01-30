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
  if(digitalRead(pbM) == 1 && digitalRead(pbH) == 0) {
    digitalWrite(ledM, HIGH);
  } else {
  	digitalWrite(ledM, LOW);
  }
  
  if(digitalRead(pbM) == 0 && digitalRead(pbH) == 1) {
    digitalWrite(ledH, HIGH);
  } else {
  	digitalWrite(ledH, LOW);
  }
  
  if(digitalRead(pbM) == 1 && digitalRead(pbH) == 1) {
    digitalWrite(ledK, HIGH);
  } else {
  	digitalWrite(ledK, LOW);
  }
}