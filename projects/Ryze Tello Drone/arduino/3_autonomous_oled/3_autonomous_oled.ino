#include <WiFi.h>
#include <WiFiUdp.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

const char* ssid = "TELLO-9C71B4";  // Nama Wi-Fi drone
const char* password = "";

WiFiUDP udp;
const char* TELLO_IP = "192.168.10.1";
const int TELLO_PORT = 8889;
const int LOCAL_PORT = 9000;  // Port untuk menerima respons

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void setup() {
    Serial.begin(115200);
    
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println("OLED gagal diinisialisasi!");
        while (true);
    }

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);

    WiFi.begin(ssid, password);
    updateOLED("Menghubungkan ke Tello...");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
    }
    updateOLED("Terhubung ke Tello!");

    // Mulai UDP
    udp.begin(LOCAL_PORT);

    // Masuk ke mode SDK
    sendCommand("command");
    delay(1000);

    // Perintah Takeoff
    sendCommand("takeoff");
    delay(5000);  // Tunggu drone stabil di udara

    // Putar 360 derajat (clockwise)
    sendCommand("cw 360");
    delay(5000);  // Tunggu drone menyelesaikan rotasi

    // Perintah Landing
    sendCommand("land");
}

void loop() {
    receiveResponse();
    delay(1000);  // Cek respons setiap 1 detik
}

void sendCommand(const char* cmd) {
    udp.beginPacket(TELLO_IP, TELLO_PORT);
    udp.print(cmd);
    udp.endPacket();
    updateOLED(String("Kirim: ") + cmd);
}

void receiveResponse() {
    int packetSize = udp.parsePacket();
    if (packetSize) {
        char buffer[255];
        int len = udp.read(buffer, 255);
        if (len > 0) {
            buffer[len] = '\0';  // Tambahkan terminator string
        }
        updateOLED(String("Respon: ") + buffer);
    }
}

void updateOLED(String text) {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println(text);
    display.display();
}