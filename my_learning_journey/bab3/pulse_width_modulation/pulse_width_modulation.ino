const int pot = A0;
const int ledH = 3;
int p;

void setup() {
  Serial.begin(9600);
  pinMode(pot,INPUT);
  pinMode(ledH,OUTPUT);
}

void loop() {
  p = analogRead(pot);
  p = map(p,0,1023,0,255);
  Serial.println(p);
  analogWrite(ledH,p);
}