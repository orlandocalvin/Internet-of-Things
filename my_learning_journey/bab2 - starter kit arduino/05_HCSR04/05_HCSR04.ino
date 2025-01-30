#include <Wire.h> 
#include <HCSR04.h>
#include <LiquidCrystal_I2C.h>

HCSR04 hc(2, 3);
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  // Serial.begin(9600);
  lcd.begin();
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("Distance :");
}

void loop() {
  lcd.setCursor(0,1);
  int dist = hc.dist();
  lcd.print( String(dist) + " CM " );
  // Serial.println(hc.dist());
  delay(500);
}
