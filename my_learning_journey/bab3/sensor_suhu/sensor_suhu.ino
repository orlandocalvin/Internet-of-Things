#include <dht.h>
#include <LiquidCrystal_I2C.h>
dht DHT11;
LiquidCrystal_I2C lcd(0x27,16,2);

const int pinDht = A0;

void setup() {
  lcd.init();
  lcd.clear();
  lcd.backlight();
  Serial.begin(9600);
}

void loop() {
  DHT11.read11(pinDht);
  String temp = String(DHT11.temperature,0);
  String hmd = String(DHT11.humidity,0);
  tampilLcd(temp,hmd);
  tampilSerial(temp,hmd);
  delay(2000);
}

void tampilLcd(String temp,String hmd) {
  lcd.setCursor(0,0);
  lcd.print("Suhu : " + temp + "C");
  lcd.setCursor(0,1);
  lcd.print("Kelembaban : " + hmd + "%");
}

void tampilSerial(String temp,String hmd) {
  Serial.println("Suhu : " + temp + "C");
  Serial.println("Kelembaban : " + hmd + "%");
  Serial.println("");
}