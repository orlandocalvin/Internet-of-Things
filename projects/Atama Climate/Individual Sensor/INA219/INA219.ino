#include <Wire.h>
#include <Adafruit_INA219.h>

Adafruit_INA219 ina219(0x41);  // INA219 sensor object

void setup(void) {
  Serial.begin(115200);

  // Initialize INA219 sensor
  if (!ina219.begin()) {
    Serial.println("Failed to find INA219 chip");
    while (1) { delay(10); }  // Stop if sensor not found
  }
}

void loop(void) {
  // Variables to store sensor readings
  float busVoltage_V = ina219.getBusVoltage_V(); // Bus voltage
  float current_mA = ina219.getCurrent_mA();     // Current in mA
  float power_mW = ina219.getPower_mW();         // Power in mW

  // Print readings to Serial Monitor
  Serial.print("Voltage:   "); Serial.print(busVoltage_V); Serial.println(" V");
  Serial.print("Current:   "); Serial.print(current_mA); Serial.println(" mA");
  Serial.print("Power:     "); Serial.print(power_mW); Serial.println(" mW");
  Serial.println();

  delay(2000);
}