const int pir = 2;
const int buzz = 3;

void setup() {
  pinMode(pir, INPUT);
  pinMode(buzz, OUTPUT);
}

void loop() {
  if(digitalRead(pir)) {
    buzzer(); 
  }
  delay(1000);
}

void buzzer() {
  for(int i=0;i<2;i++) {
    digitalWrite(buzz, HIGH);
    delay(100);
    digitalWrite(buzz, LOW);
    delay(100);
  }
}