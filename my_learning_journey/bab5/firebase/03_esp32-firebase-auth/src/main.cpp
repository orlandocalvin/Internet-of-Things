#define ENABLE_USER_AUTH

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <FirebaseClient.h>
#include "ExampleFunctions.h" // utility functions

// WiFi credentials
#define WIFI_SSID "SUPER-ORCA"
#define WIFI_PASSWORD "zxcvbnmv"

// Firebase credentials
#define API_KEY "AIzaSyBsTCTUfmyTyD25NKP7bglEa1Dg8Q2Jrnk"
#define USER_EMAIL "orlandocalvin46@gmail.com"
#define USER_PASS "11112002wso"

// Firebase objects
FirebaseApp app;
WiFiClientSecure ssl_client;
AsyncClientClass aClient(ssl_client);
UserAuth user_auth(API_KEY, USER_EMAIL, USER_PASS);
AsyncResult dbResult;

bool taskComplete = false;

// Handle Firebase authentication async result and print debug info
void handleAuthResponse(AsyncResult &result)
{
    // Return if there is no result from Firebase
    if (!result.isResult())
        return;

    // Print event info if the result is an event
    if (result.isEvent())
        Firebase.printf("Event: %s | msg: %s | code: %d\n",
                        result.uid().c_str(),
                        result.eventLog().message().c_str(),
                        result.eventLog().code());

    // Print debug message if available
    if (result.isDebug())
        Firebase.printf("Debug: %s | msg: %s\n",
                        result.uid().c_str(),
                        result.debug().c_str());

    // Print error info if the result contains an error
    if (result.isError())
        Firebase.printf("Error: %s | msg: %s | code: %d\n",
                        result.uid().c_str(),
                        result.error().message().c_str(),
                        result.error().code());

    // Print payload data if result is available
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
    initializeApp(aClient, app, getAuth(user_auth), handleAuthResponse, "authTask");
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

    // Once authenticated, print auth info
    if (app.ready() && !taskComplete)
    {
        taskComplete = true;
        Serial.println("\nAuthentication Info:");
        Firebase.printf("User UID: %s\n", app.getUid().c_str());
        Firebase.printf("Auth Token: %s\n", app.getToken().c_str());
        Firebase.printf("Refresh Token: %s\n", app.getRefreshToken().c_str());
        print_token_type(app);
    }
}