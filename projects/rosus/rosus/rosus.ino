#include <Wire.h>
#include <WiFi.h>
#include <WiFiManager.h>
#include <ESPAsyncWebServer.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Sensor.h>
#include <ArduinoJson.h>
#include <LittleFS.h>

AsyncWebServer server(80);
WiFiManager wifiManager;
Adafruit_MPU6050 mpu;
Adafruit_SSD1306 display(128, 32, &Wire, -1);

// Buffer untuk filter moving average
const int sampleCount = 10;
float accX_samples[sampleCount], accY_samples[sampleCount], accZ_samples[sampleCount];
float gyroX_samples[sampleCount], gyroY_samples[sampleCount], gyroZ_samples[sampleCount];
float temp_samples[sampleCount];
int sampleIndex = 0;

// Variabel global untuk menyimpan rata-rata akselerasi
float accX_avg = 0, accY_avg = 0, accZ_avg = 0;
float gyroX_avg = 0, gyroY_avg = 0, gyroZ_avg = 0;
float temp_avg = 0;

void getFilteredData() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  // Menyimpan data ke buffer moving average
  accX_samples[sampleIndex] = a.acceleration.x;
  accY_samples[sampleIndex] = a.acceleration.y;
  accZ_samples[sampleIndex] = a.acceleration.z;
  
  gyroX_samples[sampleIndex] = g.gyro.x;
  gyroY_samples[sampleIndex] = g.gyro.y;
  gyroZ_samples[sampleIndex] = g.gyro.z;
  temp_samples[sampleIndex] = temp.temperature;

  sampleIndex = (sampleIndex + 1) % sampleCount;

  // Menghitung rata-rata dari setiap sumbu akselerasi, giroskop, dan suhu
  accX_avg = accY_avg = accZ_avg = gyroX_avg = gyroY_avg = gyroZ_avg = temp_avg = 0;
  for (int i = 0; i < sampleCount; i++) {
    accX_avg += accX_samples[i];
    accY_avg += accY_samples[i];
    accZ_avg += accZ_samples[i];
    gyroX_avg += gyroX_samples[i];
    gyroY_avg += gyroY_samples[i];
    gyroZ_avg += gyroZ_samples[i];
    temp_avg += temp_samples[i];
  }
  accX_avg /= sampleCount;
  accY_avg /= sampleCount;
  accZ_avg /= sampleCount;
  gyroX_avg /= sampleCount;
  gyroY_avg /= sampleCount;
  gyroZ_avg /= sampleCount;
  temp_avg /= sampleCount;
}

void handleData(AsyncWebServerRequest *request) {
  StaticJsonDocument<256> data;
  getFilteredData();

  // Roll dan Pitch | satuan radian | mau derajat? * 180 / PI
  float roll = atan2(accY_avg, sqrt(accX_avg * accX_avg + accZ_avg * accZ_avg));
  float pitch = atan2(-accX_avg, accZ_avg);

  // Menyusun JSON data yang akan dikirimkan
  data["roll"] = roll;
  data["pitch"] = pitch;
  data["yaw"] = 0;  // Yaw membutuhkan kompas untuk perhitungan

  data["gyroscope"]["x"] = gyroX_avg;
  data["gyroscope"]["y"] = gyroY_avg;
  data["gyroscope"]["z"] = gyroZ_avg;

  data["acceleration"]["x"] = accX_avg;
  data["acceleration"]["y"] = accY_avg;
  data["acceleration"]["z"] = accZ_avg;

  data["temperature"] = temp_avg;
  data["wifi_ssid"] = WiFi.SSID();
  data["wifi_local_ip"] = WiFi.localIP().toString();

  String jsonString;
  serializeJson(data, jsonString);
  request->send(200, "application/json", jsonString);
}

void handleDisconnect(AsyncWebServerRequest *request) {
  request->send(200, "text/plain", "Disconnected from WiFi.");
  wifiManager.resetSettings();
  delay(1000);
  ESP.restart();
}

void setup() {
  Serial.begin(115200);
  LittleFS.begin();

  // Menyambungkan ke WiFi
  if (!wifiManager.autoConnect("Rosus AP")) {
    delay(1000);
    ESP.restart();
  }

  // Mengonfigurasi halaman statis dari LittleFS
  if (LittleFS.exists("/index.html")) {
    server.serveStatic("/", LittleFS, "/").setDefaultFile("index.html");
  } else {
    Serial.println("index.html not found");
  }

  server.on("/data", HTTP_GET, handleData);
  server.on("/disconnect", HTTP_GET, handleDisconnect);
  server.begin();

  // Inisialisasi sensor MPU6050 dan display OLED
  if (!mpu.begin()) {
    Serial.println("Sensor init failed");
    while (1) yield();
  }
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;) ;
  }

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.clearDisplay();
  display.println("SSID: " + WiFi.SSID());
  display.display();
  delay(2000);
}

void loop() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  // Menampilkan data pada layar OLED
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("IP: " + WiFi.localIP().toString());
  display.println("\nGyroscope - rps\n" + String(g.gyro.x, 1) + ", " + String(g.gyro.y, 1) + ", " + String(g.gyro.z, 1));
  display.display();
  delay(100);
}