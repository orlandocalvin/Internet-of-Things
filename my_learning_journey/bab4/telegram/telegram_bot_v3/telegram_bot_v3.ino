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

int ledPin[] = {2, 4, 5};
bool ledState[] = {LOW, LOW, LOW};
String state[] = {"", "", ""};

void setup() {
  for(int i=0;i<3;i++) {
    pinMode(ledPin[i], OUTPUT);
  }
  
  Serial.begin(115200);
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
    String commands = "Use the following commands to control LED with ESP32 \n\n"
                      "/toggle1 to turn ON/OFF LED1 \n"
                      "/toggle2 to turn ON/OFF LED2 \n"
                      "/toggle3 to turn ON/OFF LED3 \n"
                      "/state to request LED state \n";

    if (text == "/start") {
      String welcome = "Welcome, " + from_name + "!\n";
      welcome += commands;
      bot.sendMessage(chat_id, welcome, "");
    }
    
    else if (text == "/toggle1") {
      toggle(0, chat_id);
    } else if (text == "/toggle2") {
      toggle(1, chat_id);
    } else if (text == "/toggle3") {
      toggle(2, chat_id);
    }
    
    else if (text == "/state") {
      for(int i=0;i<3;i++) {
        state[i] = (ledState[i] == HIGH) ? "ON" : "OFF";
      }
      String pinState = "LED1 is " + state[0] + "\nLED2 is " + state[1] + "\nLED3 is " + state[2];
      bot.sendMessage(chat_id, pinState, "");
    }
    
    else {
      bot.sendMessage(chat_id, commands, "");
    }
  }
}

void toggle(int i, String chat_id) {
  ledState[i] = !ledState[i];
  state[i] = (ledState[i] == HIGH) ? "ON" : "OFF";
  digitalWrite(ledPin[i], ledState[i]);
  bot.sendMessage(chat_id, "LED"+ String(i+1) +" state set to "+ state[i], "");
}


