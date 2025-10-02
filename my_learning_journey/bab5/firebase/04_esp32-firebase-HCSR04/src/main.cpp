#define ENABLE_USER_AUTH
#define ENABLE_DATABASE

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <FirebaseClient.h>
#include <HCSR04.h> // Ultrasonic sensor library

// Wi-Fi credentials
#define WIFI_SSID "SUPER-ORCA"
#define WIFI_PASSWORD "zxcvbnmv"

// Firebase credentials
#define API_KEY "AIzaSyBsTCTUfmyTyD25NKP7bglEa1Dg8Q2Jrnk"
#define DATABASE_URL "https://esp-project-a1ed5-default-rtdb.firebaseio.com/"
#define USER_EMAIL "orlandocalvin46@gmail.com"
#define USER_PASS "11112002wso"

// Firebase objects
FirebaseApp app;
WiFiClientSecure ssl_client;
AsyncClientClass aClient(ssl_client);
RealtimeDatabase Database;
UserAuth user_auth(API_KEY, USER_EMAIL, USER_PASS);

// Ultrasonic sensor pins
#define TRIG 4
#define ECHO 5
UltraSonicDistanceSensor distanceSensor(TRIG, ECHO);

// Timer for sending data every 10s
unsigned long previousTime = 0;
const unsigned long interval = 10000;

// User UID and database paths
String uid;
String databasePath;
String distancePath;

// Distance value
float distanceValue = 0.0;

// Handle async response
void handleFirebaseResponse(AsyncResult &result)
{
  if (!result.isResult())
    return;

  if (result.isEvent())
    Firebase.printf("Event: %s | msg: %s | code: %d\n",
                    result.uid().c_str(),
                    result.eventLog().message().c_str(),
                    result.eventLog().code());

  if (result.isDebug())
    Firebase.printf("Debug: %s | msg: %s\n",
                    result.uid().c_str(),
                    result.debug().c_str());

  if (result.isError())
    Firebase.printf("Error: %s | msg: %s | code: %d\n",
                    result.uid().c_str(),
                    result.error().message().c_str(),
                    result.error().code());

  if (result.available())
    Firebase.printf("Task: %s | payload: %s\n",
                    result.uid().c_str(),
                    result.c_str());
}

// Connect to Wi-Fi
void connectWiFi()
{
  Serial.print("Connecting to Wi-Fi");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected! IP: ");
  Serial.println(WiFi.localIP());
}

// Setup Firebase
void setupFirebase()
{
  ssl_client.setInsecure();
  ssl_client.setHandshakeTimeout(5);
  initializeApp(aClient, app, getAuth(user_auth), handleFirebaseResponse, "authTask");
  app.getApp<RealtimeDatabase>(Database);
  Database.url(DATABASE_URL);
}

// Send ultrasonic data
void sendUltrasonicData()
{
  // Read distance in cm
  distanceValue = distanceSensor.measureDistanceCm();

  // Build user-specific path
  uid = app.getUid().c_str();
  databasePath = "UsersData/" + uid;
  distancePath = databasePath + "/distance";

  Serial.println("\nSending distance to: " + distancePath);
  Serial.println("Distance: " + String(distanceValue) + " cm");

  // Send data to Firebase
  Database.set<float>(aClient, distancePath, distanceValue, handleFirebaseResponse, "Send_Distance");
}

void setup()
{
  Serial.begin(115200);
  connectWiFi();
  setupFirebase();
}

void loop()
{
  app.loop();

  // Send data every 10 seconds
  if (app.ready())
  {
    unsigned long currentTime = millis();
    if (currentTime - previousTime >= interval)
    {
      previousTime = currentTime;
      sendUltrasonicData();
    }
  }
}