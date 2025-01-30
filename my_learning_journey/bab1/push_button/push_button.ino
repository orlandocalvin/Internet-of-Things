const int led = 3;
const int pb = 4;

int statusTombol = 0;
bool statusLed = false;

void setup() {
  pinMode(led, OUTPUT);
  pinMode(pb, INPUT);
}

void loop() {
  statusTombol = digitalRead(pb);
  if(statusTombol == HIGH) {
  	statusLed =! statusLed;
    //delay(100);
  }
  digitalWrite(led, statusLed);
}