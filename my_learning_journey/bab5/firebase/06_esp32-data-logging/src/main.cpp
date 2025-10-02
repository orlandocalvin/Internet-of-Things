#define ENABLE_USER_AUTH
#define ENABLE_DATABASE

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <FirebaseClient.h>
#include "time.h"

// Wi-Fi credentials
#define WIFI_SSID "SUPER-ORCA"
#define WIFI_PASSWORD "zxcvbnmv"

// Firebase credentials
#define API_KEY "AIzaSyBeHJC02JLGuYI4_t-br5QjkySj6e0_LU8"
#define DATABASE_URL "https://orca-esp32-firebase-default-rtdb.asia-southeast1.firebasedatabase.app/"
#define USER_EMAIL "kejow51749@auslank.com"
#define USER_PASS "kejow51749"

// Firebase objects
FirebaseApp app;
WiFiClientSecure ssl_client;
AsyncClientClass aClient(ssl_client);
RealtimeDatabase Database;
UserAuth user_auth(API_KEY, USER_EMAIL, USER_PASS);

// Timer variables
unsigned long previousTime = 0;
const unsigned long interval = 10000; // 10 seconds

// Firebase path variables
String uid;
String databasePath;
String parentPath;
String tempPath = "/temperature";
String humPath = "/humidity";
String presPath = "/pressure";
String timePath = "/timestamp";

// Dummy sensor values
float temperature = 0.0;
float humidity = 0.0;
float pressure = 0.0;

// Timestamp config
const char *ntpServer = "pool.ntp.org";
int timestamp;

// Handle async Firebase response
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

// Get current epoch time
unsigned long getTime()
{
    time_t now;
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo))
        return 0;
    time(&now);
    return now;
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

// Send dummy sensor data with timestamp
void sendLoggedData()
{
    // Generate dummy sensor values
    temperature = random(200, 350) / 10.0; // 20.0 - 35.0 °C
    humidity = random(300, 800) / 10.0;    // 30.0 - 80.0 %
    pressure = random(9500, 10500) / 10.0; // 950.0 - 1050.0 hPa

    // Build database path
    uid = app.getUid().c_str();
    databasePath = "UsersData/" + uid + "/logs";

    // Get timestamp and create parent path
    timestamp = getTime();
    parentPath = databasePath + "/" + String(timestamp);

    Serial.println("\nLogging data to: " + parentPath);
    Serial.println("Temperature: " + String(temperature) + " °C");
    Serial.println("Humidity: " + String(humidity) + " %");
    Serial.println("Pressure: " + String(pressure) + " hPa");

    // Send data with timestamp
    Database.set<float>(aClient, parentPath + tempPath, temperature, handleFirebaseResponse, "Send_Temperature");
    Database.set<float>(aClient, parentPath + humPath, humidity, handleFirebaseResponse, "Send_Humidity");
    Database.set<float>(aClient, parentPath + presPath, pressure, handleFirebaseResponse, "Send_Pressure");
    Database.set<int>(aClient, parentPath + timePath, timestamp, handleFirebaseResponse, "Send_Timestamp");
}

void setup()
{
    Serial.begin(115200);
    connectWiFi();
    configTime(0, 0, ntpServer);
    setupFirebase();
}

void loop()
{
    app.loop();

    // Send logged data every 10 seconds
    if (app.ready())
    {
        unsigned long currentTime = millis();
        if (currentTime - previousTime >= interval)
        {
            previousTime = currentTime;
            sendLoggedData();
        }
    }
}