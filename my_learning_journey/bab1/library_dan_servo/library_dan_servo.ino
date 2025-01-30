/* Coding Sederhana Penggunaan Library pada Servo
#include <Servo.h>
Servo servo;

void setup() {
  servo.attach(2);
}

void loop() {
  servo.write(90);
  delay(1000);
  servo.write(0);
  delay(1000); 
}*/

#include <Servo.h>
Servo servo;

const int pServo = 2;
const int pbKanan = 3;
const int pbKiri = 4;
int dtcKa = 0;
int dtcKi = 0;
int sudut = 0;

void setup() {
  // Serial.begin(9600);
  servo.attach(pServo);
  pinMode(pbKanan, INPUT);
  pinMode(pbKiri, INPUT);
}

void loop() {
  dtcKa = digitalRead(pbKanan);
  dtcKi = digitalRead(pbKiri);
  
  if(dtcKa == 1) {
    sudut++;
    if(sudut > 180) {
      sudut = 180;
    }
    servo.write(sudut);
  }
  if(dtcKi == 1) {
    sudut--;
    if(sudut < 0) {
      sudut = 0;
    }
    servo.write(sudut);
  }
  // Serial.println(sudut);
}