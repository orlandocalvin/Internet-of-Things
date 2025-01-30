#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,16,2);

// const int pot = A0;
// int nilaiPot;

void setup() {
  lcd.begin();
  lcd.backlight();
  
  // pinMode(pot,INPUT);
  
  lcd.setCursor(0,0);
  lcd.print("Hello World!");
}

void loop() {
  // nilaiPot = analogRead(pot);
  // lcd.setCursor(0,0);
  // lcd.print("Nilai Potensio :");
  // lcd.setCursor(0,1);
  // lcd.println(nilaiPot);

  /* Running Text
  lcd.scrollDisplayLeft();
  delay(300);
  */

}