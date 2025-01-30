// digitalRead
// ------------
// digitalWrite

const int pb = 2;
const int led = 3;
bool rPb;

void setup() {
  Serial.begin(9600);
  pinMode(pb, INPUT);
  pinMode(led, OUTPUT);
}

void loop() {
  rPb = digitalRead(pb);
  if(rPb == 1) {
    digitalWrite(led, HIGH);
    Serial.println("LED Hidup");
  	delay(2000);
  } else {
    digitalWrite(led, LOW);
    Serial.println("LED Mati");
  	delay(2000);
  }
}