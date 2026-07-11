#include <HardwareSerial.h>

HardwareSerial simSerial(1);

void sendAT(String cmd) {
  Serial.println("\n>> " + cmd);
  simSerial.println(cmd); // send to SIM7600X
  delay(1000);

  while (simSerial.available()) {
    Serial.write(simSerial.read()); // print raw response
  }
  Serial.println("----------------------");
}

void setup() {
  Serial.begin(115200);
  simSerial.begin(115200, SERIAL_8N1, 16, 17); // RX=16, TX=17
  delay(2000);
  
  Serial.println("=== SIM7600X Connection Test ===");

  // 1. ESP32 <-> SIM7600X
  sendAT("AT");        // response: OK
  sendAT("ATI");       // module info

  // 2. SIM Card
  sendAT("AT+CPIN?");  // SIM ready → +CPIN: READY
  sendAT("AT+CSQ");    // check signal
  sendAT("AT+COPS?");  // the registered operator

  // 3. Internet Connection
  sendAT("AT+CGATT?"); // attach GPRS (1 = attached)
  sendAT("AT+CGDCONT=1,\"IP\",\"indosatgprs\""); // change APN according to operator
  sendAT("AT+CGACT=1,1"); // activate PDP context
  sendAT("AT+CGPADDR=1"); // check obtained IP address

  Serial.println("=== Test completed ===");
}

void loop() {
  if (simSerial.available()) { // Show module response
    String line = simSerial.readStringUntil('\n');
    Serial.println(line);
  }
}