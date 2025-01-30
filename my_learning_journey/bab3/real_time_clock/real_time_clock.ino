#include <DS3231.h>
DS3231 rtc(SDA,SCL);

const int relay = 1;
String hari, jam, tanggal;

void setup() {
  rtc.begin();
  Serial.begin(9600);
  pinMode(relay,OUTPUT);
  digitalWrite(relay, LOW);
  // rtc.setDOW(THURSDAY);
  // rtc.setTime(13,41,0);
  // rtc.setDate(3,8,2023);
}

void loop() {
  hari = rtc.getDOWstr();
  jam = rtc.getTimeStr();
  tanggal = rtc.getDateStr();
  // Serial.println(hari+", "+jam+", "+tanggal);\
  Serial.println(jam);
  if(jam == "13:45:00") {
    digitalWrite(relay, HIGH);
    delay(2000);
    digitalWrite(relay, LOW);
  }
  delay(1000);
}