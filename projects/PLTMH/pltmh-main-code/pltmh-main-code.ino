#define ENABLE_USER_AUTH
#define ENABLE_DATABASE

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <FirebaseClient.h>
#include <time.h>
#include <Wire.h>
#include <Adafruit_INA219.h>

// Wi-Fi & Firebase Credentials
#define WIFI_SSID "PLTMH-001"
#define WIFI_PASSWORD "pltmh-001"

#define API_KEY "AIzaSyBlEgmF5ViEQpIMny71RwGS7J2s2vprAbs"
#define DATABASE_URL "https://pltmh-project-default-rtdb.asia-southeast1.firebasedatabase.app/"
#define USER_EMAIL "pltmh-001@gmail.com"
#define USER_PASS "pltmh-001"

// === RPM dari Hall A3144 ===
#define HALL_PIN 4
#define PULSES_PER_REV 1
#define DEBOUNCE_US 800
#define WINDOW_MS 1000

// Filter
#define EMA_ALPHA 0.30f
#define RPM_MAX_DELTA 100.0f

volatile uint32_t hallPulseCount = 0;
volatile uint32_t lastPulseUs = 0;

// Firebase Objects
FirebaseApp app;
WiFiClientSecure ssl_client;
AsyncClientClass aClient(ssl_client);
RealtimeDatabase Database;
UserAuth user_auth(API_KEY, USER_EMAIL, USER_PASS);

// Timer & Path Variables
unsigned long previousTime = 0;
const unsigned long interval = 5000; // 5s

String uid;
String databasePath;
String parentPath;
String voltPath = "/voltage";
String currPath = "/current";
String powerPath = "/power";
String rpmPath = "/rpm";
String timePath = "/timestamp";

Adafruit_INA219 ina219(0x41);

// Sensor Data
float voltage = 0.0;
float current = 0.0;
float power = 0.0;
float rpmRaw = 0.0f;
float rpmFiltered = 0.0f;
float rpmPrev = 0.0f;

uint32_t lastRpmCalcMs = 0;

// Time Configuration
const char *ntpServer = "pool.ntp.org";
uint32_t timestamp;
unsigned long readCounter = 0; // log counter

void IRAM_ATTR hallISR()
{
    uint32_t nowUs = micros();
    if (nowUs - lastPulseUs > DEBOUNCE_US)
    {
        hallPulseCount++;
        lastPulseUs = nowUs;
    }
}

// Firebase Response Handler
void handleFirebaseResponse(AsyncResult &result)
{
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

// Get current time (epoch)
unsigned long getTime()
{
    time_t now;
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo))
        return 0;
    time(&now);
    return now;
}

// Initialize Firebase
void setupFirebase()
{
    ssl_client.setInsecure(); // no certificate validation
    ssl_client.setHandshakeTimeout(5);
    initializeApp(aClient, app, getAuth(user_auth), handleFirebaseResponse, "authTask");
    app.getApp<RealtimeDatabase>(Database);
    Database.url(DATABASE_URL);
}

// Send Data to RTDB
void sendLoggedData()
{
    float busV = ina219.getBusVoltage_V();     // Volt
    float current_mA = ina219.getCurrent_mA(); // mA
    float power_mW = ina219.getPower_mW();     // mW

    // Simple conversion & fallback if NaN
    voltage = isfinite(busV) ? busV : 0.0f;
    current = isfinite(current_mA) ? current_mA / 1000.0f : 0.0f;          // A
    power = isfinite(power_mW) ? power_mW / 1000.0f : (voltage * current); // W
    int rpmToSend = (int)roundf(rpmFiltered);

    readCounter++;

    // Guard UID
    uid = app.getUid().c_str();
    if (uid.length() == 0)
    {
        Serial.println();
        Serial.printf("[LOG %lu] UID is empty, skip sending...\n", readCounter);
        Serial.println();
        return;
    }

    // Build path
    databasePath = "UsersData/" + uid + "/logs";
    timestamp = getTime();
    parentPath = databasePath + "/" + String((uint32_t)timestamp);

    // Print log
    Serial.println();
    Serial.printf("[LOG %lu]\n", readCounter);
    Serial.printf("path: %s\n", parentPath.c_str());
    Serial.printf("volt=%.1f V  curr=%.1f A  power=%.1f W  rpm=%d  ts=%lu\n", voltage, current, power, rpmToSend, (unsigned long)timestamp);
    Serial.println();

    // Write data to RTDB
    Database.set<float>(aClient, parentPath + voltPath, voltage, handleFirebaseResponse, "Send_Voltage");
    Database.set<float>(aClient, parentPath + currPath, current, handleFirebaseResponse, "Send_Current");
    Database.set<float>(aClient, parentPath + powerPath, power, handleFirebaseResponse, "Send_Power");
    Database.set<int>(aClient, parentPath + rpmPath, rpmToSend, handleFirebaseResponse, "Send_RPM");
    Database.set<int>(aClient, parentPath + timePath, timestamp, handleFirebaseResponse, "Send_Timestamp");
}

void updateRpm()
{
    uint32_t nowMs = millis();
    if (nowMs - lastRpmCalcMs < WINDOW_MS)
        return;

    noInterrupts();
    uint32_t pulses = hallPulseCount;
    hallPulseCount = 0;
    interrupts();

    float windowSec = (nowMs - lastRpmCalcMs) / 1000.0f;
    lastRpmCalcMs = nowMs;

    rpmRaw = (windowSec > 0) ? (pulses / (float)PULSES_PER_REV) * (60.0f / windowSec) : rpmRaw;

    rpmFiltered = (EMA_ALPHA * rpmRaw) + ((1.0f - EMA_ALPHA) * rpmFiltered);

    float delta = rpmFiltered - rpmPrev;
    if (delta > RPM_MAX_DELTA)
        rpmFiltered = rpmPrev + RPM_MAX_DELTA;
    if (delta < -RPM_MAX_DELTA)
        rpmFiltered = rpmPrev - RPM_MAX_DELTA;

    rpmPrev = rpmFiltered;
}

void setup()
{
    Serial.begin(115200);
    Wire.begin();
    connectWiFi();

    // Wait for NTP time to sync (max ~5s)
    configTzTime("WIB-7", ntpServer);
    for (int i = 0; i < 50 && getTime() < 1700000000UL; i++)
        delay(100);

    // Init INA219 + kalibrasi
    if (!ina219.begin())
        Serial.println("INA219 not found!");
    else
        ina219.setCalibration_16V_400mA(); // ina219.setCalibration_32V_2A();

    pinMode(HALL_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(HALL_PIN), hallISR, FALLING);
    lastRpmCalcMs = millis();

    setupFirebase();
}

void loop()
{
    // Firebase app loop
    app.loop();
    updateRpm();

    // Send data
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