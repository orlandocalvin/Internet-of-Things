#include <DHT.h>

#define DHTPIN 27        // DHT22 Pin
#define DHTTYPE DHT22   // DHT Type (DHT11, DHT22)

// Object Initialization
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  dht.begin();
  
  Serial.println("Waiting for DHT22 sensor...");
  delay(2000);
}

void loop() {
  // Read Humidity & Temperature
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
  
  // Check Sensor Reading
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT22 sensor!");
    return;
  }
  
  // Calculate Heat Index
  float heatIndex = dht.computeHeatIndex(temperature, humidity, false);
  
  // Show Results
  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.print(" %");
  
  Serial.print(" | Temp: ");
  Serial.print(temperature);
  Serial.print(" °C");
  
  Serial.print(" | Heat Index: ");
  Serial.print(heatIndex);
  Serial.println(" °C");
  
  delay(2000);
}