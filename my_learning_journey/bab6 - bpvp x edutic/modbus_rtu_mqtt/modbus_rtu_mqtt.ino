#include <WiFi.h>
#include <ModbusMaster.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

// Modbus configuration
#define MODBUS_EN_PIN 4       // DE and RE pin
#define MODBUS_RO_PIN 18       // RO pin
#define MODBUS_DI_PIN 19       // DI pin
#define MODBUS_SERIAL_BAUD 9600 // Baud rate for Modbus
#define MODBUS_PARITY SERIAL_8N1 // Parity for Modbus

//SHT20 configuration
#define MODBUS_SLAVE_ID 1      // Slave ID for Modbus
#define MODBUS_ADDRESS 0x0001  // Modbus address to fetch data
#define MODBUS_QUANTITY 2      // Number of registers to fetch

// WiFi and MQTT configuration
const char* ssid = "SUPER-ORCA";
const char* password = "zxcvbnmv";
const char* mqtt_server = "broker.emqx.io";
#define PUBLISH_TOPIC "orca/xy-md02"


WiFiClient espClient;
PubSubClient mqtt(espClient);
ModbusMaster modbus;

// Modbus pre-transmission callback
void modbusPreTransmission() {
  delay(500);
  digitalWrite(MODBUS_EN_PIN, HIGH);
}

// Modbus post-transmission callback
void modbusPostTransmission() {
  digitalWrite(MODBUS_EN_PIN, LOW);
  delay(500);
}

void setup() {
  Serial.begin(115200);
  
  pinMode(MODBUS_EN_PIN, OUTPUT);
  digitalWrite(MODBUS_EN_PIN, LOW);
  
  // Initialize Modbus communication
  Serial2.begin(MODBUS_SERIAL_BAUD, MODBUS_PARITY, MODBUS_RO_PIN, MODBUS_DI_PIN);
  Serial2.setTimeout(1000);
  modbus.begin(MODBUS_SLAVE_ID, Serial2);
  modbus.preTransmission(modbusPreTransmission);
  modbus.postTransmission(modbusPostTransmission);

  //init WiFi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi Connected!!");

  // init MQTT
  mqtt.setServer(mqtt_server, 1883);
  mqtt.connect("ESP32 MODBUS");
}

void loop() {
  int hasil[2];

  int pooling = modbus.readInputRegisters(MODBUS_ADDRESS, MODBUS_QUANTITY);

  if (pooling == modbus.ku8MBSuccess) {
    Serial.println("Success, Data diterima:");
    
    hasil[0] = modbus.getResponseBuffer(0x00);
    hasil[1] = modbus.getResponseBuffer(0x01);
      
    
    float suhu = hasil[0] / 10.f;
    float kelembaban = hasil[1] / 10.f;
    
    Serial.println("Suhu: " + String(suhu));
    Serial.println("Kelembaban: " + String(kelembaban));
    
    JsonDocument doc;
    char buffer[10];

    sprintf(buffer, "%0.1f", suhu);
    doc["temperature"] = buffer;

    sprintf(buffer, "%0.1f", kelembaban);
    doc["humidity"] = buffer;

    char json[256];
    serializeJson(doc, json);
    
    Serial.println("Send data to : " + String(PUBLISH_TOPIC));
    mqtt.publish(PUBLISH_TOPIC, json);
    
    Serial.println();
    
  } else {
    Serial.println("GAGAL membaca data");
  }

  delay(2000); // Delay between Modbus requests
}