#include <EEPROM.h>
byte nilai;

void setup() {
  Serial.begin(9600);
  EEPROM.write(0,222);
  EEPROM.write(1,255);
  EEPROM.write(9,167);
}

void loop() {
  nilai = EEPROM.read(0);
  Serial.println(nilai);
  nilai = EEPROM.read(1);
  Serial.println(nilai);
  nilai = EEPROM.read(9);
  Serial.println(nilai);
  Serial.println("");
  delay(3000);
}