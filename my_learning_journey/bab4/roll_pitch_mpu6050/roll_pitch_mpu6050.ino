#include <Wire.h>
#include <Adafruit_MPU6050.h>

Adafruit_MPU6050 mpu;

float Roll, Pitch;

void setup() {
  Serial.begin(9600);
  
  // Inisialisasi sensor MPU6050
  if (!mpu.begin()) {
    Serial.println("Gagal menginisialisasi MPU6050");
    while (1);
  }

  Serial.println("MPU6050 siap");
}

void loop() {
  // Membaca data akselerometer dan giroskop
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  
  // Menghitung Roll dan Pitch
  Roll  = atan2(a.acceleration.y, sqrt(a.acceleration.x * a.acceleration.x + a.acceleration.z * a.acceleration.z)) * 180.0 / PI;
  Pitch = atan2(-a.acceleration.x, a.acceleration.z) * 180.0 / PI;

  // Menampilkan hasil
  Serial.print("Pitch: "); Serial.print(Pitch);
  Serial.print(" Roll: "); Serial.println(Roll);

  delay(500);
}
