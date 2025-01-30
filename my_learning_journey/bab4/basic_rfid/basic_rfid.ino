/* Wiring:
  SDA (SS): Pin 10
  SCK: Pin 13
  MOSI: Pin 11
  MISO: Pin 12
  IRQ: Tidak terhubung
  GND: GND
  RST: Pin 9
  3.3V: 3.3V */

#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10  // Pin SDA pada RFID-RC522
#define RST_PIN 9  // Pin RST pada RFID-RC522
#define PIN_LED 2

MFRC522 rfid(SS_PIN, RST_PIN); // Inisialisasi objek MFRC522

void setup() {
  Serial.begin(9600);   // Buka komunikasi serial
  SPI.begin();          // Inisialisasi SPI bus
  rfid.PCD_Init();      // Inisialisasi modul RFID
  Serial.println("Scan tag RFID untuk membaca UID...");
  pinMode(PIN_LED, OUTPUT);
}

void loop() {
  // Cek apakah ada kartu/tagnya
  if (!rfid.PICC_IsNewCardPresent()) {
    return;
  }

  // Cek apakah tag sudah bisa dibaca
  if (!rfid.PICC_ReadCardSerial()) {
    return;
  }

  String ID = "";

  // Tampilkan UID yang dibaca dari RFID
  Serial.print("UID RFID:");
  for (byte i = 0; i < rfid.uid.size; i++) {
    // Data mentah UID
    // ID += String(rfid.uid.uidByte[i]);

    // Menampilkan nilai UID dalam format hexadecimal dengan tampilan yang rapi
    ID += String(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
    ID += String(rfid.uid.uidByte[i], HEX);
  }
  ID.toUpperCase();
  Serial.println(ID);

  // UID Card: (HEX) => 43 AC 52 34, (INT) => 671728252
  // UID Tag: (HEX) => 43 5F FE 13, (INT) => 679525419

  if (ID == " 43 AC 52 34"){
    digitalWrite(PIN_LED, HIGH);
  } else if(ID == " 43 5F FE 13"){
    digitalWrite(PIN_LED, LOW);
  }

  rfid.PICC_HaltA();      // Menghentikan komunikasi dengan tag RFID
  rfid.PCD_StopCrypto1(); // Hentikan enkripsi
}
