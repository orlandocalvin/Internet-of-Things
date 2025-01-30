#include <esp_now.h>
#include <WiFi.h>

// 1 (receiver) = CC:8D:A2:8C:42:04
uint8_t broadcastAddress[] = {0xCC, 0x8D, 0xA2, 0x8C, 0x42, 0x04};

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_peer_info_t peerInfo;
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }
}

void loop() {
  const char* data = "Hello from sender";
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)data, strlen(data));
  if (result == ESP_OK) {
    Serial.println("Sent with success");
  } else {
    Serial.println("Error sending the data");
  }
  delay(1000);
}
