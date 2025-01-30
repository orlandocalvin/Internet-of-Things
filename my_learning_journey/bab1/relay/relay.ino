const int relay = 2;

void setup() {
  pinMode(relay,OUTPUT);
}

void loop() {
  digitalWrite(relay, HIGH);
  delay(800);
  digitalWrite(relay, LOW);
  delay(800);
}