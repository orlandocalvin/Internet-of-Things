#include <esp_now.h>
#include <WiFi.h>

void onDataRecv(const esp_now_recv_info *info, const uint8_t *incomingData, int len) {
  char receivedData[len + 1];
  memcpy(receivedData, incomingData, len);
  receivedData[len] = '\0';

  Serial.print("Received from: ");
  for (int i = 0; i < 6; i++) {
    Serial.printf("%02X", info->src_addr[i]);
    if (i < 5) Serial.print(":");
  }
  Serial.print(" - Data: ");
  Serial.println(receivedData);
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  esp_now_register_recv_cb(onDataRecv);
}

void loop() {
  // Tidak ada yang dilakukan di loop
}
