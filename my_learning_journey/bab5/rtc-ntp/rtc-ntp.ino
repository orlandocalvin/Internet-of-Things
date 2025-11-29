#include <WiFi.h>
#include <RTClib.h>
#include <time.h>

// === RTC MODULE ===
RTC_DS3231 rtc;

// === WIFI CONFIG ===
const char* WIFI_SSID     = "SUPER-ORCA";
const char* WIFI_PASSWORD = "zxcvbnmv";

// === NTP CONFIG ===
const char* NTP_SERVER    = "pool.ntp.org";
const long  GMT_OFFSET    = 7 * 3600;  // UTC+7 (Jakarta)
const int   DST_OFFSET    = 0;         // No daylight saving

// === TIMING ===
unsigned long lastDisplay = 0;
const unsigned long DISPLAY_INTERVAL = 1000; // 1 sec

// === HELPER: format 2 digits ===
String twoDigits(int n) {
  if (n < 10) return "0" + String(n);
  return String(n);
}

void setup() {
  Serial.begin(115200);
  delay(2000);
  Serial.println("\n=== RTC + NTP Clock ===");

  // Start RTC
  if (!rtc.begin()) {
    Serial.println("RTC not detected! Check wiring.");
    while (1);
  }

  // Connect WiFi
  Serial.print("Connecting to WiFi");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" WiFi connected!");

  // Get time from NTP and set RTC
  configTime(GMT_OFFSET, DST_OFFSET, NTP_SERVER);
  struct tm timeinfo;
  if (getLocalTime(&timeinfo)) {
    DateTime ntpTime = DateTime(
      timeinfo.tm_year + 1900,
      timeinfo.tm_mon + 1,
      timeinfo.tm_mday,
      timeinfo.tm_hour,
      timeinfo.tm_min,
      timeinfo.tm_sec
    );
    rtc.adjust(ntpTime);
    Serial.println("RTC synced with NTP!");
  } else {
    Serial.println("Failed to get NTP time, RTC keeps old time.");
  }

  // Disconnect WiFi (save power)
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
}

void loop() {
  unsigned long now = millis();
  if (now - lastDisplay >= DISPLAY_INTERVAL) {
    DateTime t = rtc.now();
    Serial.print("Date: ");
    Serial.print(t.year()); Serial.print("/");
    Serial.print(twoDigits(t.month())); Serial.print("/");
    Serial.print(twoDigits(t.day()));

    Serial.print("  Time: ");
    Serial.print(twoDigits(t.hour())); Serial.print(":");
    Serial.print(twoDigits(t.minute())); Serial.print(":");
    Serial.print(twoDigits(t.second()));
    Serial.println(" WIB");

    lastDisplay = now;
  }
}