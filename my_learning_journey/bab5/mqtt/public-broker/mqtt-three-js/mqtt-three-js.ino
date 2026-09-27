#include <Wire.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <AsyncMqttClient.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_MPU6050.h>

#include "secrets.h"  // credentials (gitignored; see secrets.example.h)

// MQTT broker information
#define MQTT_HOST "broker.hivemq.com"
#define MQTT_PORT 1883
#define MQTT_TOPIC "orca/rosus/3dvisual"

Adafruit_MPU6050 mpu;
AsyncMqttClient mqttClient;

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

void sendData() {
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
  mqttClient.publish(MQTT_TOPIC, 0, false, jsonString.c_str());
}

void WiFiEvent(WiFiEvent_t event) {
  switch(event) {
    case ARDUINO_EVENT_WIFI_STA_GOT_IP:
      Serial.print("WiFi connected! Connecting to MQTT... ");
      mqttClient.connect();  // Connect to MQTT server
      break;
    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
      Serial.println("WiFi lost, reconnecting...");
      WiFi.begin(WIFI_SSID, WIFI_PASSWORD);  // Reconnect to Wi-Fi  
      break;
  }
}

void onMqttConnect(bool sessionPresent) {
  Serial.println("Connected to MQTT!");
}

void setup() {
  Serial.begin(115200);
  WiFi.onEvent(WiFiEvent);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  // Setup MQTT Client
  mqttClient.onConnect(onMqttConnect);
  mqttClient.setServer(MQTT_HOST, MQTT_PORT);

  if (!mpu.begin()) {
    Serial.println("Sensor init failed");
    while (1) delay(100);
  }
}

void loop() {
  (!mqttClient.connected()) ? mqttClient.connect() : sendData();
  delay(15);
}