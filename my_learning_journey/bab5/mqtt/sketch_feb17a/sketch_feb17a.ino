#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "REDACTED";
const char* password = "REDACTED";
const char* mqtt_server = "broker.hivemq.com";
// const char* mqtt_server = "test.mosquitto.org";
const char* topic = "orca/topic";

WiFiClient espClient;
PubSubClient client(espClient);

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message received on topic: ");
  Serial.println(topic);

  Serial.print("Message: ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void connectWiFi() {
  Serial.print("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\nWiFi connected");
}

void connectMQTT() {
  while (!client.connected()) {
    Serial.print("Connecting to MQTT...");
    if (client.connect("ESP32_Client")) {
      Serial.println("connected!");
      // client.publish(topic, "Hello from ESP32");
      client.subscribe(topic);
    } else {
      Serial.print("failed, rc=");
      Serial.println(client.state());
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  connectWiFi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  connectMQTT();
}

void loop() {
  if (!client.connected()) connectMQTT();
  client.loop();

  static unsigned long lastMsg = 0;
  if (millis() - lastMsg > 1000) {
    client.publish(topic, "Hello from ESP32");
    lastMsg = millis();
  }
}