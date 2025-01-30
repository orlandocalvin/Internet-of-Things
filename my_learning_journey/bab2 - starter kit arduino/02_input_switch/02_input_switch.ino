int led = 7;
int button = 3;

void setup() {
  pinMode(led, OUTPUT);
  pinMode(button, INPUT);
}

void loop() {
  if(digitalRead(button)) {
    digitalWrite(led, HIGH);
  } else {
    digitalWrite(led, LOW);
  }
}