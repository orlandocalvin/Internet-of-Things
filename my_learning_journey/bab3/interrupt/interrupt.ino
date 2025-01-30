#include<TimerOne.h>

const int red=2,green=1,pb=3;

void setup() {
  pinMode(red,OUTPUT);
  pinMode(green,OUTPUT);
  pinMode(pb,INPUT);
  Timer1.initialize(10000);
  Timer1.attachInterrupt(blinkRed);
}

void loop() {
  for(int i=0;i<5;i++) {
    digitalWrite(green, HIGH);
    delay(700);
    digitalWrite(green, LOW);
    delay(700);
  }
}

void blinkRed() {
  if(digitalRead(pb) == HIGH) {
    digitalWrite(red, HIGH);
  } else {
    digitalWrite(red, LOW);
  }
}