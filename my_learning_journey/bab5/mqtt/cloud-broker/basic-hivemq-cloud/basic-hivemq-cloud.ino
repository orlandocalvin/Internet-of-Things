#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>

#include "secrets.h"  // credentials (gitignored; see secrets.example.h)

#define MQTT_HOST "728895e7c26149d4981edb8564e5e159.s1.eu.hivemq.cloud"
#define MQTT_PORT 8883


WiFiClientSecure net;
PubSubClient mqttClient(net);

void connectToWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi!");
}

void connectToMQTT() {
  mqttClient.setServer(MQTT_HOST, MQTT_PORT);
  while (!mqttClient.connected()) {
    Serial.print("Connecting to MQTT...");
    if (mqttClient.connect("ESP32Client", MQTT_USERNAME, MQTT_PASSWORD)) {
      Serial.println("Connected!");
      mqttClient.subscribe("test/topic");
    } else {
      Serial.print("Failed, rc=");
      Serial.println(mqttClient.state());
      delay(5000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message received on topic: ");
  Serial.println(topic);
  Serial.print("Payload: ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void setup() {
  Serial.begin(115200);
  connectToWiFi();

  net.setInsecure();
  mqttClient.setCallback(callback);

  connectToMQTT();
}

void loop() {
  mqttClient.loop();

  if(!mqttClient.connected()) {
    Serial.println("Pedot Bang!");
    connectToMQTT();
  }

  static unsigned long lastPublish = 0;
  if (mqttClient.connected() && millis() - lastPublish > 1000) {
    lastPublish = millis();
    mqttClient.publish("test/topic", "Hello from ESP32!");
  }
}