#include <HardwareSerial.h>

HardwareSerial simSerial(1);

void sendAT(String cmd, unsigned long wait = 2000) { // send AT command with simple delay
  Serial.println("\n>> " + cmd);
  simSerial.println(cmd); // send to SIM7600X
  delay(wait);

  while (simSerial.available()) {
    Serial.write(simSerial.read()); // print raw response
  }
  Serial.println("----------------------");
}

void setup() {
  Serial.begin(115200);
  simSerial.begin(115200, SERIAL_8N1, 16, 17); // RX=16, TX=17
  delay(3000);

  Serial.println("=== SIM7600X HTTPS Test (GET) ===");

  // basic checks
  sendAT("AT");
  sendAT("ATI");
  sendAT("AT+CPIN?");
  sendAT("AT+CSQ");

  // set APN
  sendAT("AT+CGDCONT=1,\"IP\",\"indosatgprs\"");
  sendAT("AT+CGATT=1", 5000);
  sendAT("AT+CGACT=1,1", 5000);
  sendAT("AT+CGPADDR=1");

  // https request
  sendAT("AT+HTTPINIT", 3000);
  sendAT("AT+HTTPPARA=\"URL\",\"https://httpbin.org/get\"");
  sendAT("AT+HTTPACTION=0", 10000);  // 0 = GET
  sendAT("AT+HTTPREAD=0,512", 5000);
  sendAT("AT+HTTPTERM");

  Serial.println("=== Test done ===");
}

void loop() {
  if (simSerial.available()) { // keep showing async response if any
    Serial.write(simSerial.read());
  }
}