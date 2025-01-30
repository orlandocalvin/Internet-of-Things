#include <Servo.h>
Servo servo;

void setup() {
  servo.attach(2);
  pinMode(A0, INPUT);
}

void loop() {
  // Knob
  servo.write(
    map(analogRead(A0), 0,1023,0,180)
  );
  delay(15);

  // Sweep
  // for (int i = 0; i <= 180; i++) {
  //   delay(15);
  //   servo.write(i);
  // }

  // for (int j = 180; j >= 0; j--) {
  //   servo.write(j);
  //   delay(15);
  // }
}