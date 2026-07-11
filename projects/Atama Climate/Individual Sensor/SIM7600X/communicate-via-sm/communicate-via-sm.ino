#include <HardwareSerial.h>

HardwareSerial simSerial(1); // UART1 for SIM7600X

void setup() {
  Serial.begin(115200);
  simSerial.begin(115200, SERIAL_8N1, 16, 17); // RX=16, TX=17
  Serial.println("ESP32 Ready. Type AT commands in Serial Monitor.");
}

void loop() {
  // Forward from Serial Monitor to SIM7600X
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    simSerial.println(cmd);
  }

  // Forward from SIM7600X to Serial Monitor
  if (simSerial.available()) {
    String res = simSerial.readStringUntil('\n');
    Serial.println(res);
  }
}