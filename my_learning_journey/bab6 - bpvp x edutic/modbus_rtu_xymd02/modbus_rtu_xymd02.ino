#include <ModbusMaster.h>

// Modbus pins
#define MODBUS_EN_PIN  4
#define MODBUS_RO_PIN 16
#define MODBUS_DI_PIN 17

// Modbus settings
#define MODBUS_SERIAL_BAUD 9600
#define MODBUS_PARITY SERIAL_8N1
#define MODBUS_SLAVE_ID 1
#define MODBUS_ADDRESS 0x0001
#define MODBUS_QUANTITY 2

ModbusMaster modbus;

// Enable TX mode
void modbusPreTransmission() {
  delay(500);
  digitalWrite(MODBUS_EN_PIN, HIGH);
}

// Enable RX mode
void modbusPostTransmission() {
  digitalWrite(MODBUS_EN_PIN, LOW);
  delay(500);
}

void setup() {
  Serial.begin(115200);
  pinMode(MODBUS_EN_PIN, OUTPUT);
  digitalWrite(MODBUS_EN_PIN, LOW);

  Serial2.begin(MODBUS_SERIAL_BAUD, MODBUS_PARITY, MODBUS_RO_PIN, MODBUS_DI_PIN);
  Serial2.setTimeout(1000);

  modbus.begin(MODBUS_SLAVE_ID, Serial2);
  modbus.preTransmission(modbusPreTransmission);
  modbus.postTransmission(modbusPostTransmission);
}

void loop() {
  int status;
  int buffer[2];
  float temperature;
  float humidity;

  // Read Modbus registers
  status = modbus.readInputRegisters(MODBUS_ADDRESS, MODBUS_QUANTITY);

  if (status == modbus.ku8MBSuccess) {
    Serial.println("Data received:");

    buffer[0] = modbus.getResponseBuffer(0x00);
    buffer[1] = modbus.getResponseBuffer(0x01);

    temperature = buffer[0] / 10.0f;
    humidity    = buffer[1] / 10.0f;

    Serial.println("Temperature: " + String(temperature));
    Serial.println("Humidity: " + String(humidity));
    Serial.println();
  } else {
    Serial.println("Failed to read data");
  }

  delay(1000);
}