#include <NewPing.h>
NewPing sensor(3,2,30);
const int led = 4;
int jarak;

void setup() {
  Serial.begin(9600);
  pinMode(led, OUTPUT);
  delay(50);
}

void loop() {
  jarak = sensor.ping_cm();
  if(jarak <= 5) {
    digitalWrite(led, HIGH);
    delay(50);
    digitalWrite(led, LOW);
    delay(50);
  } else {
    digitalWrite(led, HIGH);
    delay(500);
    digitalWrite(led, LOW);
    delay(500);
  }
  Serial.println(jarak);
}