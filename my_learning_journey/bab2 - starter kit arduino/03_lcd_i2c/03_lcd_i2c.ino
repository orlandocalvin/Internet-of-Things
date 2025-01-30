#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  lcd.begin();
  lcd.backlight();
  pinMode(A0, INPUT);
}

void loop() {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Nilai Potensio :");
  lcd.setCursor(6,1);
  lcd.print(map(analogRead(A0),0,1023,0,180));
  delay(150);
}