#include <Wire.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_SSD1306.h>

#define OLED_WIDTH 128
#define OLED_HEIGHT 32
#define SAMPLE_COUNT 10
#define RECEIVER_PORT 1234
#define RECEIVER_IP "192.168.4.1"
const char* SSID = "ESP8266 RC CAR";
#define PASSWORD "zxcvbnmv"

Adafruit_MPU6050 mpu;
Adafruit_SSD1306 oled(OLED_WIDTH, OLED_HEIGHT, &Wire, -1);

WiFiUDP udp;

float accX_samples[SAMPLE_COUNT], accY_samples[SAMPLE_COUNT], accZ_samples[SAMPLE_COUNT]; // Buffer untuk filter moving average
float accX_avg = 0, accY_avg = 0, accZ_avg = 0; // Variabel global untuk menyimpan rata-rata akselerasi
int sampleIndex = 0;

void getFilteredData() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  // Menyimpan data ke buffer moving average
  accX_samples[sampleIndex] = a.acceleration.x;
  accY_samples[sampleIndex] = a.acceleration.y;
  accZ_samples[sampleIndex] = a.acceleration.z;
  
  sampleIndex = (sampleIndex + 1) % SAMPLE_COUNT;

  // Menghitung rata-rata dari akselerasi
  accX_avg = accY_avg = accZ_avg = 0;
  for (int i = 0; i < SAMPLE_COUNT; i++) {
    accX_avg += accX_samples[i];
    accY_avg += accY_samples[i];
    accZ_avg += accZ_samples[i];
  }
  accX_avg /= SAMPLE_COUNT;
  accY_avg /= SAMPLE_COUNT;
  accZ_avg /= SAMPLE_COUNT;
}

void setup() {
  Serial.begin(115200);
  mpu.begin();

  WiFi.begin(SSID, PASSWORD);
  delay(3000);

  oled.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  oled.setRotation(2); // rotate text 180⁰
  oled.setTextSize(1);
  oled.setTextColor(WHITE);
  oled.clearDisplay();
  oled.setCursor(0,0);
  oled.println("Can't Connect to \nReceiver");

  if(WiFi.status() == WL_CONNECTED) {
    oled.clearDisplay();
    oled.setCursor(0,0);
    oled.println("SSID: " + WiFi.SSID());
    oled.println("\nIP: " + WiFi.localIP().toString());
  }
    oled.display();
}

void loop() {
  StaticJsonDocument<256> data;
  getFilteredData();

  // Roll dan Pitch | satuan derajat | mau radian? hilangkan * 180 / PI
  float roll = atan2(accY_avg, sqrt(accX_avg * accX_avg + accZ_avg * accZ_avg)) * 180 / PI;
  float pitch = atan2(-accX_avg, accZ_avg) * 180 / PI;

  // Menyusun JSON data yang akan dikirimkan
  data["roll"] = roll;
  data["pitch"] = pitch;

  String jsonString;
  serializeJson(data, jsonString);

  udp.beginPacket(RECEIVER_IP, RECEIVER_PORT);
  udp.print(jsonString);
  udp.endPacket();

  delay(15);
}