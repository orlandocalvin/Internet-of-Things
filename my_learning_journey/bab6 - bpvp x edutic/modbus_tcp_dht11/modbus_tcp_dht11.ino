#include <WiFi.h>
#include <ModbusIP_ESP8266.h>
#include <DHT.h>

#define DHTPIN 15
#define DHTTYPE DHT11
#define RELAY1_PIN 25
#define RELAY2_PIN 26

#define TEMPERATURE_ADDRESS 100
#define HUMIDITY_ADDRESS 101
#define RELAY1_Coil 102
#define RELAY2_Coil 103

const char* ssid = "SUPER-ORCA";
const char* pass = "zxcvbnmv";

DHT dht(DHTPIN, DHTTYPE);
ModbusIP mb;

void setup() {
  Serial.begin(115200);
  Serial.println(F("MODBUS TCP OVER WIFI ESP32"));

  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Terhubung!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  dht.begin();

  pinMode(RELAY1_PIN, OUTPUT);
  pinMode(RELAY2_PIN, OUTPUT);
  digitalWrite(RELAY1_PIN, HIGH);
  digitalWrite(RELAY2_PIN, HIGH);

  mb.server();

  mb.addHreg(TEMPERATURE_ADDRESS);
  mb.addHreg(HUMIDITY_ADDRESS);
  mb.addCoil(RELAY1_Coil);
  mb.addCoil(RELAY2_Coil);

  mb.Coil(RELAY1_Coil, 0);
  mb.Coil(RELAY2_Coil, 0);
}

void loop() {

  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  if (!isnan(humidity) && !isnan(temperature)) {
    mb.Hreg(TEMPERATURE_ADDRESS, temperature * 10);
    mb.Hreg(HUMIDITY_ADDRESS, humidity * 10);
  }

  mb.task();


  bool relay1_state = (mb.Coil(RELAY1_Coil) == 1) ? LOW : HIGH;
  bool relay2_state = (mb.Coil(RELAY2_Coil) == 1) ? LOW : HIGH;

  digitalWrite(RELAY1_PIN, relay1_state);
  digitalWrite(RELAY2_PIN, relay2_state);

  delay(100);
}