#include <Wire.h>
#include <RTClib.h>
#include <LiquidCrystal_I2C.h>

RTC_DS1307 rtc;
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  lcd.init();
  lcd.backlight();
  
  if (!rtc.begin()) {
    Serial.println("RTC tidak terdeteksi!");
    while (1);
  }

  // Atur waktu hanya sekali (komentar setelah waktu diatur)
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
}

void loop() {
  DateTime now = rtc.now();

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Tgl: " + String(now.day()) + "/" + String(now.month()) + "/" + String(now.year()));

  lcd.setCursor(0, 1);
  lcd.print("Waktu: " + String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second()));

  delay(1000);
}
