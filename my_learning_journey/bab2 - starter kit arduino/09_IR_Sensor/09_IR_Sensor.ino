#include <Wire.h>
#include <LiquidCrystal_I2C.h>

const int sensor = 4;
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  pinMode(sensor, INPUT);
  
  lcd.begin();
  lcd.backlight();

  lcd.setCursor(0,0);
  lcd.print("par1 : isi");
  lcd.setCursor(0,1);
  lcd.print("par2 :");
}

void loop() {
  lcd.setCursor(7,1);

  if(digitalRead(sensor)) {
    lcd.print("kosong ");
  } else {
    lcd.print("isi   ");
  }

}