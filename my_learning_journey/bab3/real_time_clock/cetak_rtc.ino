#include <LiquidCrystal.h>
#include <DS3231.h>
LiquidCrystal lcd(7,6,5,4,3,2);
DS3231 rtc(SDA,SCL);

String hari,jam,tanggal;

void setup() {
  lcd.begin(16,2);
  rtc.begin();
  // rtc.setDOW(SATURDAY);
  // rtc.setTime(10,0,0);
  // rtc.setDate(5,8,2023);
}

void loop() {
  hari = rtc.getDOWStr();
  jam = rtc.getTimeStr();
  tanggal = rtc.getTimeStr();
  tampilWaktu(hari,jam,tanggal);
  
  /* Bonus : Running Text
  lcd.ScrollDisplayLeft();
  delay(300);
  */
}

void tampilWaktu(String h,String j,String t) {
  lcd.setCursor(0,0);
  lcd.print(h);
  lcd.setCursor(8,0);
  lcd.print(j);
  lcd.setCursor(4,1);
  lcd.print(t);
}