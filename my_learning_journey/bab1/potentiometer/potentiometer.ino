#include <Servo.h>
Servo servo;

const int pServo = 2;
const int pot = A0;
int sudut;
int nPot;

void setup() {
  // Serial.begin(9600);
  servo.attach(pServo);
  pinMode(pot, INPUT);
}

void loop() {
  nPot = analogRead(pot);
  sudut = map(nPot,0,1023,0,180);
  servo.write(sudut);
  // Serial.println(sudut);
}