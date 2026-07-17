#include <WiFi.h>
#include <WiFiUdp.h>

const char* ssid = "TELLO-9C71B4";
const char* password = "";

WiFiUDP udp;
const char* TELLO_IP = "192.168.10.1";
const int TELLO_PORT = 8889;
const int LOCAL_PORT = 9000;

void setup() {
    Serial.begin(115200);
    WiFi.begin(ssid, password);

    Serial.print("Menghubungkan ke Tello");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nTerhubung ke Tello!");

    // Mulai UDP
    udp.begin(LOCAL_PORT);

    // Masuk ke mode SDK
    sendCommand("command");
    delay(1000);

    // Minta status baterai
    sendCommand("battery?");
}

void loop() {
    receiveResponse();
    delay(1000);
}

void sendCommand(const char* cmd) {
    udp.beginPacket(TELLO_IP, TELLO_PORT);
    udp.print(cmd);
    udp.endPacket();
    Serial.print("Mengirim perintah: ");
    Serial.println(cmd);
}

void receiveResponse() {
    int packetSize = udp.parsePacket();
    if (packetSize) {
        char buffer[255];
        int len = udp.read(buffer, 255);
        if (len > 0) {
            buffer[len] = '\0';  // Null Terminator
        }
        Serial.print("Respons dari Tello: ");
        Serial.println(buffer);
    }
}