#include <Wire.h>
#include <BH1750.h>
#include <LiquidCrystal_I2C.h>

// ---------------------------
// Setup sensor & LCD
// ---------------------------
BH1750 lightMeter(0x23);
LiquidCrystal_I2C lcd(0x27, 20, 4); // alamat 0x27, 20 kolom x 4 baris

void setup() {
  Serial.begin(115200);
  Wire.begin();

  // Inisialisasi BH1750
  if (!lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE)) {
    Serial.println("Failed to initialize BH1750!");
    while (1);
  }

  // Inisialisasi LCD
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("BH1750 Lux Monitor");
  delay(2000);
}

void loop() {
  // Baca lux
  float lux = lightMeter.readLightLevel();

  // Serial monitor
  Serial.print("Light: "); Serial.print(lux); Serial.println(" lux");

  // Tampilkan ke LCD 20x4
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("BH1750 Lux Sensor");
  
  lcd.setCursor(0,1);
  lcd.print("Illuminance: ");
  lcd.print(lux, 2); // 2 desimal
  lcd.print(" lx");

  delay(1000); // update tiap 1 detik
}