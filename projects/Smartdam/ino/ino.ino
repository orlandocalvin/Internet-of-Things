#include <dht.h>
#include <WiFi.h>
#include <HCSR04.h>
#include <ESP32Servo.h>
#include <HTTPClient.h>

const char *ssid = "acer";
const char *password = "12345678";

dht DHT11;
Servo servo1;
HCSR04 hc(18,19);

String status, temp, hmd, postData, rainStatus;
int jarak, ketinggian, rainValue;
const int buzzer = 4;
const int pinDht = 5;
const int rainDrop = 34;
const int tinggi_sensor = 19;

void setup() {
  servo1.attach(22);
  servo1.write(0);
  DHT11.read11(pinDht);
  Serial.begin(115200);
  pinMode(buzzer, OUTPUT);
  pinMode(rainDrop, INPUT);
  WiFi.begin(ssid, password);
}

void loop() {
  jarak = hc.dist();
  ketinggian = tinggi_sensor - jarak;
  hmd = String(DHT11.humidity,0);
  temp = String(DHT11.temperature,0);
  rainValue = map(analogRead(rainDrop),1000,2000,4,1);

  bacaData();
  cekKetinggianAir(ketinggian);
  tampilSerial(status,ketinggian,hmd,temp,rainValue);

  postData = "ketinggian=" + (String)ketinggian + "&suhu=" + temp + "&kelembaban=" + hmd + "&rain_value=" + rainStatus;
  kirimData(postData);

  delay(1000);
}

void bacaData() {
  HTTPClient http;
  http.begin("https://orlandocalvin.000webhostapp.com/smartdam/json/servo.json");

  http.GET();
  String json = http.getString();
  if(json == "{\"servo\": \"on\"}") {
    bukaBendungan();
  }
  http.end();
}

void kirimData(String postData) {
  HTTPClient http;
  http.begin("https://orlandocalvin.000webhostapp.com/smartdam/api/api_smartdam.php");
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  http.POST(postData);
  http.end();
}

void cekKetinggianAir(int ketinggian) {
  if(ketinggian <= 7) {
    status = "Aman";
  } else if(ketinggian > 7 && ketinggian <= 10) {
    status = "Waspada!";
  } else if(ketinggian > 10 && ketinggian <= 12) {
    status = "Siaga!";
  } else if(ketinggian > 12) {
    status = "Bahaya!";
    bukaBendungan();
  }
}

void bukaBendungan() {
  int pos = 0;
  bool buzzerState = LOW;

  Serial.println("\nAWAS! BENDUNGAN DIBUKA!\n");
  for(int i = 0; i < 6; i++) {
    buzzerState = !buzzerState;
    digitalWrite(buzzer, buzzerState);
    delay(500);
  }
  
  for (pos = 0; pos <= 180; pos += 1) {
		servo1.write(pos);
		delay(30);
	}
	for (pos = 180; pos >= 0; pos -= 1) {
		servo1.write(pos);
		delay(15);
	}
}

void tampilSerial(String status, int ketinggian, String hmd, String temp, int rainValue) {
  if(rainValue == 1) {
    rainStatus = "Gerimis";
  } else if(rainValue == 2 ) {
    rainStatus = "Hujan Ringan";
  } else if(rainValue >= 3) {
    rainStatus = "Hujan Lebat";
  } else {
    rainStatus = "Tidak Hujan";
  }

  Serial.println("Suhu : " + temp + " C");
  Serial.println("Kelembaban : " + hmd + "%");
  Serial.println("Rain Status : " + rainStatus + "\n");
  Serial.println("Ketinggian Air = "+ (String)ketinggian + " CM");
  Serial.println("Status Bendungan = " + (String)status + "\n");
}