#include <Wire.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_SSD1306.h>

#define OLED_WIDTH 128
#define OLED_HEIGHT 32
#define SENDER_ID 102 // Perhatikan SENDER_ID dari masing" device
#define SAMPLE_COUNT 10
#define RECEIVER_PORT 4321
#define RECEIVER_IP "10.42.0.1"
#define SSID "REDACTED"
#define PASSWORD "REDACTED"

Adafruit_MPU6050 mpu;
Adafruit_SSD1306 oled(OLED_WIDTH, OLED_HEIGHT, &Wire, -1);

WiFiUDP udp;

IPAddress localIP(10, 42, 0, SENDER_ID);
IPAddress gateway(10, 42, 0, 1);
IPAddress subnet(255, 255, 255, 0);

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

void sendData() {
  StaticJsonDocument<256> data;
  getFilteredData();

  // Roll dan Pitch | satuan radian | mau derajat? * 180 / PI
  float roll = atan2(accY_avg, sqrt(accX_avg * accX_avg + accZ_avg * accZ_avg)) * 180 / PI;
  float pitch = atan2(-accX_avg, accZ_avg) * 180 / PI;

  // Menyusun JSON data yang akan dikirimkan
  data["sender_id"] = SENDER_ID;
  data["roll"] = roll;
  data["pitch"] = pitch;

  String jsonString;
  serializeJson(data, jsonString);

  udp.beginPacket(RECEIVER_IP, RECEIVER_PORT);
  udp.print(jsonString);
  udp.endPacket();
}

void setup() {
  Serial.begin(115200);
  mpu.begin();

  WiFi.config(localIP, gateway, subnet);
  WiFi.begin(SSID, PASSWORD);
  delay(3000);

  oled.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  oled.setTextSize(1);
  oled.setTextColor(WHITE);
  oled.clearDisplay();
  oled.setCursor(0,0);
  oled.println("Can't Connect to \nReceiver");

  if(WiFi.status() == WL_CONNECTED) {
    oled.clearDisplay();
    oled.setCursor(0,0);
    oled.println("Rosus Type: Sender");
    oled.println("\nSender ID: " + String(SENDER_ID));
    oled.println("IP: " + WiFi.localIP().toString());
  }
    oled.display();
}

void loop() {
  sendData();
  delay(90);
}