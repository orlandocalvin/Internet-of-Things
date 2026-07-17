#include <WiFi.h>

const char* ssid = "TELLO-9C71B4";
const char* password = "";

void setup() {
    Serial.begin(115200);
    WiFi.begin(ssid, password);

    Serial.print("Menghubungkan ke Tello");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nTerhubung ke Tello!");
    Serial.print("IP ESP32-C3: ");
    Serial.println(WiFi.localIP());
}

void loop() {
    // Tidak ada loop, hanya koneksi awal
}