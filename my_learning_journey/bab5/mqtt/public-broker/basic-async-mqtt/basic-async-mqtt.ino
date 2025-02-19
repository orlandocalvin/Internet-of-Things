#include <WiFi.h>
#include <AsyncMqttClient.h>

#define WIFI_SSID "REDACTED"
#define WIFI_PASSWORD "REDACTED"

// MQTT broker information
#define MQTT_HOST "broker.hivemq.com"
// #define MQTT_HOST "test.mosquitto.org"
#define MQTT_PORT 1883
#define MQTT_TOPIC "orca/test"

AsyncMqttClient mqttClient;

void WiFiEvent(WiFiEvent_t event) {
  switch(event) {
    case ARDUINO_EVENT_WIFI_STA_GOT_IP:
      Serial.print("WiFi connected! Connecting to MQTT... ");
      mqttClient.connect();  // Connect to MQTT server
      break;
    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
      Serial.println("WiFi lost, reconnecting...");
      WiFi.begin(WIFI_SSID, WIFI_PASSWORD);  // Reconnect to Wi-Fi  
      break;
  }
}

void onMqttConnect(bool sessionPresent) {
  Serial.println("Connected to MQTT!");
  mqttClient.subscribe(MQTT_TOPIC, 0);  // Subscribe to MQTT topic
}

void onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {
  Serial.println("Message received on topic: " + String(topic));
  Serial.print("Message: ");
  for (size_t i = 0; i < len; i++) {
    Serial.print((char)payload[i]);  // Print received message
  }
  Serial.println();
}

void setup() {
  Serial.begin(115200);
  WiFi.onEvent(WiFiEvent);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  // Setup MQTT Client
  mqttClient.onConnect(onMqttConnect);
  mqttClient.onMessage(onMqttMessage);
  mqttClient.setServer(MQTT_HOST, MQTT_PORT);
}

void loop() {
  static unsigned long lastPublish = 0;
  if (mqttClient.connected() && millis() - lastPublish > 1000) {
    lastPublish = millis();
    mqttClient.publish(MQTT_TOPIC, 0, false, "Hello from ESP32!");  // Publish message to MQTT
  }
}