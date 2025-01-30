#include <Wire.h>
#include <Keypad.h>
#include <Keypad_I2C.h>
#include <LiquidCrystal_I2C.h>

#define I2CADDR 0x20

const byte ROWS = 4;
const byte COLS = 4;

char hexaKeys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {7, 6, 5, 4};
byte colPins[COLS] = {3, 2, 1, 0};

LiquidCrystal_I2C lcd(0x27, 16, 2);
Keypad_I2C keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS, I2CADDR); 

void setup() {
  lcd.begin();
  Wire.begin();
  keypad.begin();
  lcd.backlight();
}

void loop() {
  lcd.setCursor(0,0);
  lcd.print("key-pressed :");
  char key = keypad.getKey();

  lcd.setCursor(14,0);
  if(key) {
    lcd.print(key);
  }

}