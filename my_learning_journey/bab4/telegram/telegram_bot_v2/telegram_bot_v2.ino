#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>

// Initialize Telegram BOT
#include "secrets.h"  // credentials (gitignored; see secrets.example.h)


WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

// Checks for new messages every 1 second.
int botRequestDelay = 1000;
unsigned long lastTimeBotRan;

const int ledPin = 2;
bool ledState = digitalRead(ledPin);

void setup() {
  Serial.begin(115200);

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, ledState);
  
  WiFi.begin(ssid, password);

  // Add root certificate for api.telegram.org
  client.setCACert(TELEGRAM_CERTIFICATE_ROOT);
}

void loop() {
  if (millis() > lastTimeBotRan + botRequestDelay)  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while(numNewMessages) {
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastTimeBotRan = millis();
  }
}

// Handle when you receive new messages
void handleNewMessages(int numNewMessages) {
  for (int i=0; i<numNewMessages; i++) {

    // Chat id of the requester
    String chat_id = String(bot.messages[i].chat_id);
    if (chat_id != CHAT_ID){
      bot.sendMessage(chat_id, "Unauthorized user", "");
      continue;
    }
    
    String text = bot.messages[i].text;
    String from_name = bot.messages[i].from_name;
    String commands = "Use the following commands to control LED BUILTIN ESP32 \n\n"
                      "/toggle to turn ON/OFF LED \n"
                      "/state to request current LED state \n";
    String state;

    if (text == "/start") {
      String welcome = "Welcome, " + from_name + "!\n";
      welcome += commands;
      bot.sendMessage(chat_id, welcome, "");
    }
    
    else if (text == "/toggle") {
      ledState = !ledState;
      state = (ledState == HIGH) ? "ON" : "OFF";
      digitalWrite(ledPin, ledState);
      bot.sendMessage(chat_id, "LED state set to "+ state, "");
    }
    
    else if (text == "/state") {
      state = (ledState == HIGH) ? "ON" : "OFF";
      bot.sendMessage(chat_id, "LED is "+ state, "");
    }
    
    else {
      bot.sendMessage(chat_id, commands, "");
    }
  }
}