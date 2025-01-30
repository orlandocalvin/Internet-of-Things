const int ldr = A0;
const int led = 2;
int rLdr;
int knvr;

void setup() {
  Serial.begin(9600);
  pinMode(ldr, INPUT);
  pinMode(led, OUTPUT);
}

void loop() {
  rLdr = analogRead(ldr);
  knvr = map(rLdr,6,679,0,100);
  delay(300);
  Serial.println(knvr);
  if(knvr < 50) {
    digitalWrite(led, HIGH);
  } else {
    digitalWrite(led, LOW);
  }

  /* Advance LDR
  int ldr = map(analogRead(A0),0,60,255,0);
  if(ldr>50) {
    analogWrite(3,ldr);
  } else if(ldr>35 && ldr<=50) {
    analogWrite(3,45);
  } else if(ldr<=35) {
    analogWrite(3,LOW);
  } */
}