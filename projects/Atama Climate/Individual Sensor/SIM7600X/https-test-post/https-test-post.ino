#include <HardwareSerial.h>

HardwareSerial simSerial(1);

// send AT command & print all response, optionally wait for OK/ERROR
void sendAT(String cmd, unsigned long wait = 2000, bool waitOK = false) {
  Serial.println("\n>> " + cmd);
  simSerial.println(cmd); // send to SIM7600X
  delay(wait);

  String response = "";
  while (simSerial.available()) {
    response += char(simSerial.read());
  }
  Serial.print(response);
  Serial.println("----------------------");

  if (waitOK) {
    unsigned long start = millis();
    while (millis() - start < 15000) { // max 15s
      while (simSerial.available()) {
        response += char(simSerial.read());
        if (response.indexOf("OK") != -1 || response.indexOf("ERROR") != -1) {
          Serial.println("Module ready: " + response);
          return;
        }
      }
      delay(100);
    }
    Serial.println("Timeout waiting for OK/ERROR");
  }
}

// function untuk POST JSON sederhana
void httpPost(String url, String payload) {
  // terminate any existing session
  sendAT("AT+HTTPTERM", 2000);

  // init HTTP
  sendAT("AT+HTTPINIT", 3000);
  sendAT("AT+HTTPPARA=\"CID\",1");
  sendAT("AT+HTTPPARA=\"URL\",\"" + url + "\"");
  sendAT("AT+HTTPPARA=\"CONTENT\",\"application/json\"");

  // send payload
  sendAT("AT+HTTPDATA=" + String(payload.length()) + ",10000", 1000);
  delay(500);             // sedikit tunggu sebelum kirim
  simSerial.print(payload);
  delay(2000);            // tunggu modul menerima data

  // perform POST
  sendAT("AT+HTTPACTION=1", 15000);

  // read response
  sendAT("AT+HTTPREAD=0,512", 5000);

  // terminate session
  sendAT("AT+HTTPTERM", 2000);
}

void setup() {
  Serial.begin(115200);
  simSerial.begin(115200, SERIAL_8N1, 16, 17);
  delay(3000);

  Serial.println("=== SIM7600X HTTPS Test (POST) ===");

  // basic checks
  sendAT("AT");
  sendAT("AT+CPIN?");
  sendAT("AT+CSQ");
  sendAT("AT+CREG?");

  // APN & attach
  sendAT("AT+CGDCONT=1,\"IP\",\"indosatgprs\"");
  sendAT("AT+CGATT=1", 5000);
  sendAT("AT+CGACT=1,1", 8000);
  sendAT("AT+CGPADDR=1");

  // contoh POST
  String payload = "{\"sensor\":\"temperature\",\"value\":25.6,\"unit\":\"celsius\"}";
  httpPost("https://httpbin.org/post", payload);

  Serial.println("=== Test completed ===");
}

void loop() {
  if (simSerial.available()) {
    Serial.write(simSerial.read());
  }
}