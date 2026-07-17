#define ENABLE_USER_AUTH
#define ENABLE_DATABASE

// ============================================================================
// LIBRARIES
// ============================================================================

// Core libraries
#include <Arduino.h>
#include <WiFi.h>
#include <FirebaseClient.h>
#include <WiFiClientSecure.h>
#include <ExampleFunctions.h> // Required for getAuth() and SSL_CLIENT

// Sensor libraries
#include <HCSR04.h>
#include <OneWire.h>
#include <ModbusMaster.h>
#include <LiquidCrystal_I2C.h>
#include <DallasTemperature.h>
#include <RTClib.h>

// ============================================================================
// CONFIGURATION
// ============================================================================

// --- MODE SWITCH ---
// Set 1 for Demo Mode
// Set 0 for Real Case
#define DEMO_MODE 1

// Time Conversion Functions
constexpr unsigned long SECONDS(unsigned long s)
{ // seconds to milliseconds
    return s * 1000;
}
constexpr unsigned long MINUTES(unsigned long m)
{ // minutes to milliseconds
    return m * 60 * 1000;
}

// WiFi Credentials
#define WIFI_SSID "SUPER-ORCA"
#define WIFI_PASSWORD "zxcvbnmv"
constexpr unsigned long INITIALIZE_INTERVAL = SECONDS(2);
constexpr unsigned long WIFI_CONNECT_TIMEOUT = SECONDS(10);

// Firebase Configuration
#define Web_API_KEY "AIzaSyBeHJC02JLGuYI4_t-br5QjkySj6e0_LU8"
#define DATABASE_URL "https://orca-esp32-firebase-default-rtdb.asia-southeast1.firebasedatabase.app/"
#define USER_EMAIL "user1@gmail.com"
#define USER_PASS "user1_123"

// Pin definitions
constexpr int PIN_DS18B20 = 23;
constexpr int PIN_TRIG = 5;
constexpr int PIN_ECHO = 18;
constexpr int RELAY1 = 26; // Circulation pump
constexpr int RELAY2 = 27; // lED Grow Light
constexpr int PIN_LDR = 34;

// SHT20 Modbus
constexpr int MODBUS_EN_PIN = 4;
constexpr int MODBUS_RO_PIN = 16; // RX Serial2
constexpr int MODBUS_DI_PIN = 17; // TX Serial2
constexpr unsigned long MODBUS_SERIAL_BAUD = 9600;
constexpr uint32_t MODBUS_PARITY = SERIAL_8N1;
constexpr int MODBUS_SLAVE_ID = 1;
constexpr int MODBUS_ADDRESS = 0x0001;
constexpr int MODBUS_QUANTITY = 2;

// LDR and Lux Calculation
constexpr int LDR_MIN_READING = 0;
constexpr int LDR_MAX_READING = 2400;
constexpr int MAX_LUX_VALUE = 1000;
constexpr int LUX_THRESHOLD = 333; // Below this = grow light ON

// Timing intervals
#if DEMO_MODE
constexpr unsigned long SENSOR_READ_INTERVAL = SECONDS(1);
constexpr unsigned long PUMP_ON_INTERVAL = SECONDS(10);
constexpr unsigned long LOG_UPDATE_INTERVAL = SECONDS(10);
constexpr unsigned long STATE_UPDATE_INTERVAL = SECONDS(10);

#else
constexpr unsigned long SENSOR_READ_INTERVAL = MINUTES(1);
constexpr unsigned long PUMP_ON_INTERVAL = MINUTES(10);
constexpr unsigned long LOG_UPDATE_INTERVAL = MINUTES(10);
constexpr unsigned long STATE_UPDATE_INTERVAL = MINUTES(10);
#endif

// ============================================================================
// GLOBAL OBJECTS
// ============================================================================

// Firebase
FirebaseApp app;
RealtimeDatabase Database;
UserAuth user_auth(Web_API_KEY, USER_EMAIL, USER_PASS);
SSL_CLIENT ssl_client, stream_ssl_client; // SSL_CLIENT defined in ExampleFunctions.h
AsyncClientClass aClient(ssl_client), streamClient(stream_ssl_client);

// Sensors & Actuators
RTC_DS3231 rtc;
ModbusMaster modbus;
OneWire oneWire(PIN_DS18B20);
DallasTemperature ds(&oneWire);
LiquidCrystal_I2C lcd(0x27, 16, 2);
UltraSonicDistanceSensor hcsr04(PIN_TRIG, PIN_ECHO);

// ============================================================================
// GLOBAL STATE VARIABLES
// ============================================================================

// Firebase Variables
String uid, baseUserPath, logsPath, cmdPath, statePath;
constexpr int rtdb_data_type_boolean = 4; // Boolean type identifier
bool streamStarted = false;
bool isAutoMode = true;

// Timer Variables
unsigned long lastSensorRead = 0;
unsigned long lastUpload = 0;
unsigned long lastDashboardUpload = 0;
bool pumpIsOn = false;
unsigned long pumpStartTime = 0;

// Sensor Values
float nutrientTemp = NAN;
float nutrientLevel = NAN;
float airTemp = NAN;
float airHumidity = NAN;
int luxValue = 0;

// ============================================================================
// 1. LOW-LEVEL HARDWARE UTILITIES
// ============================================================================

void setRelay(int pin, bool on)
{
    digitalWrite(pin, on ? LOW : HIGH); // Active LOW

    // Update relay state in Firebase
    if (app.ready() && !statePath.isEmpty())
    {
        String relayKey;
        if (pin == RELAY1)
            relayKey = "relay1";
        else if (pin == RELAY2)
            relayKey = "relay2";

        if (!relayKey.isEmpty())
        {
            Database.set<bool>(aClient, statePath + "/" + relayKey, on);
        }
    }
}

// Modbus SHT20 callbacks
void modbusPreTransmission()
{
    delay(5);
    digitalWrite(MODBUS_EN_PIN, HIGH);
}

void modbusPostTransmission()
{
    digitalWrite(MODBUS_EN_PIN, LOW);
    delay(5);
}

// ============================================================================
// 2. SENSOR READING & LCD
// ============================================================================

void readSHT20()
{
    uint8_t status;
    uint16_t buffer[2];

    status = modbus.readInputRegisters(MODBUS_ADDRESS, MODBUS_QUANTITY);

    if (status == modbus.ku8MBSuccess)
    {
        buffer[0] = modbus.getResponseBuffer(0x00);
        buffer[1] = modbus.getResponseBuffer(0x01);
        airTemp = buffer[0] / 10.0f;
        airHumidity = buffer[1] / 10.0f;
    }
    else
    {
        airTemp = NAN;
        airHumidity = NAN;
    }
}

void readLDR()
{
    int ldrValue = analogRead(PIN_LDR);

    // Lux Calculation
    int constrainedVal = constrain(ldrValue, LDR_MIN_READING, LDR_MAX_READING);
    luxValue = map(constrainedVal, LDR_MIN_READING, LDR_MAX_READING, 0, MAX_LUX_VALUE);
}

float readHCSR04()
{
    return hcsr04.measureDistanceCm();
}

float readDS18B20()
{
    ds.requestTemperatures();
    float nutrientTemp = ds.getTempCByIndex(0);
    return (nutrientTemp <= -120.0f || nutrientTemp == 85.0f) ? NAN : nutrientTemp;
}

void updateSensorsAndLCD()
{
    readSHT20();
    readLDR();
    nutrientLevel = readHCSR04();
    nutrientTemp = readDS18B20();

    // --- LINE 1 ---
    lcd.setCursor(0, 0);
    lcd.print("NT:"); // Nutrient Temp
    lcd.setCursor(3, 0);
    isnan(nutrientTemp) ? lcd.print("--") : lcd.print(nutrientTemp, 1);
    lcd.print("C ");

    lcd.setCursor(9, 0);
    lcd.print("NL:"); // Nutrient Level
    lcd.setCursor(12, 0);
    isnan(nutrientLevel) ? lcd.print("--") : lcd.print(nutrientLevel, 0);
    lcd.print("CM ");

    // --- LINE 2 ---
    lcd.setCursor(0, 1);
    lcd.print("AT:"); // Air Temp
    lcd.setCursor(3, 1);
    isnan(airTemp) ? lcd.print("--") : lcd.print(airTemp, 1);
    lcd.print("C ");

    lcd.setCursor(9, 1);
    lcd.print("AH:"); // Ambient Humidity
    lcd.setCursor(12, 1);
    isnan(airHumidity) ? lcd.print("--") : lcd.print(airHumidity, 0);
    lcd.print("%  ");
}

// ============================================================================
// 3. AUTOMATION LOGIC
// ============================================================================

void runPumpSchedule()
{
    DateTime now = rtc.now();

    // --- Variable Declarations ---
#if DEMO_MODE
    static int lastTriggerSecond = -1;       // to avoid multiple triggers
    int currentSecond = now.second();        // get current second
    bool triggerTime = (currentSecond == 0); // Trigger every minute at second 0
#else
    static int lastTriggerHour = -1;
    int currentHour = now.hour();
    int currentMinute = now.minute();
    // trigger at 6:00, 10:00, 14:00, 18:00
    bool triggerTime = (currentHour == 6 || currentHour == 10 || currentHour == 14 || currentHour == 18) && (currentMinute == 0);
#endif

    // --- Pump Control Logic ---
#if DEMO_MODE
    if (triggerTime && (currentSecond != lastTriggerSecond) && !pumpIsOn)
    {
        setRelay(RELAY1, true);
        pumpIsOn = true;
        pumpStartTime = millis();
        lastTriggerSecond = currentSecond; // Update last trigger second
    }
#else
    if (triggerTime && (currentHour != lastTriggerHour) && !pumpIsOn)
    {
        setRelay(RELAY1, true);
        pumpIsOn = true;
        pumpStartTime = millis();
        lastTriggerHour = currentHour;
    }
#endif

    // --- Turn Off Pump After Interval ---
    if (pumpIsOn && (millis() - pumpStartTime >= PUMP_ON_INTERVAL))
    {
        setRelay(RELAY1, false);
        pumpIsOn = false;
    }
}

void manageGrowLight()
{
    if (luxValue <= LUX_THRESHOLD)
        setRelay(RELAY2, true);
    else
        setRelay(RELAY2, false);
}

void runAutomation()
{
    runPumpSchedule();
    manageGrowLight();
}

// ============================================================================
// 4. FIREBASE & CONNECTIVITY
// ============================================================================

void uploadLogData()
{
    if (!app.ready() || uid.isEmpty())
        return;

    // Get Epoch Timestamp
    time_t timestamp = rtc.now().unixtime();
    String logPath;

    // Send data if timestamp valid
    if (timestamp > 1000000000)
    {
        logPath = logsPath + "/" + String(timestamp);
        Database.set<int>(aClient, logPath + "/timestamp", timestamp);

        if (!isnan(nutrientTemp))
            Database.set<float>(aClient, logPath + "/nutrientTemp", nutrientTemp);

        if (!isnan(nutrientLevel))
            Database.set<float>(aClient, logPath + "/nutrientLevel", nutrientLevel);

        if (!isnan(airTemp))
            Database.set<float>(aClient, logPath + "/airTemp", airTemp);

        if (!isnan(airHumidity))
            Database.set<float>(aClient, logPath + "/airHumidity", airHumidity);

        Database.set<int>(aClient, logPath + "/luxValue", luxValue);
    }
}

void uploadStateData()
{
    if (!app.ready() || uid.isEmpty() || statePath.isEmpty())
        return;

    time_t timestamp = rtc.now().unixtime();
    if (timestamp > 1000000000) // Send data if timestamp valid
        Database.set<int>(aClient, statePath + "/timestamp", timestamp);

    if (!isnan(nutrientTemp))
        Database.set<float>(aClient, statePath + "/nutrientTemp", nutrientTemp);

    if (!isnan(nutrientLevel))
        Database.set<float>(aClient, statePath + "/nutrientLevel", nutrientLevel);

    if (!isnan(airTemp))
        Database.set<float>(aClient, statePath + "/airTemp", airTemp);

    if (!isnan(airHumidity))
        Database.set<float>(aClient, statePath + "/airHumidity", airHumidity);

    Database.set<int>(aClient, statePath + "/luxValue", luxValue);
}

void handleFirebaseCommands(AsyncResult &aResult)
{
    // Exit if there's no valid data or an error occurred
    if (!aResult.isResult() || aResult.isError() || !aResult.available())
        return;

    // Convert the result to a RealtimeDatabaseResult
    RealtimeDatabaseResult &rtdb = aResult.to<RealtimeDatabaseResult>();

    // Ensure it's a stream event with boolean data
    if (!rtdb.isStream() || rtdb.type() != rtdb_data_type_boolean)
        return;

    String dataPath = rtdb.dataPath();
    bool state = rtdb.to<bool>();

    // Handle auto mode toggle
    if (dataPath == "/isAutoMode")
    {
        bool newMode = state;
        // If switching to auto mode, turn off pump immediately
        if (newMode == true && isAutoMode == false)
        {
            setRelay(RELAY1, false);
            pumpIsOn = false;
            pumpStartTime = 0;
        }

        // If switching to manual mode, update relay states in Firebase
        if (newMode == false && isAutoMode == true)
        {
            if (app.ready() && !cmdPath.isEmpty())
            {
                Database.set<bool>(aClient, cmdPath + "/relay1", digitalRead(RELAY1) == LOW);
                Database.set<bool>(aClient, cmdPath + "/relay2", digitalRead(RELAY2) == LOW);
            }
        }

        isAutoMode = newMode;

        // Update isAutoMode state in Firebase
        if (app.ready() && !statePath.isEmpty())
        {
            Database.set<bool>(aClient, statePath + "/isAutoMode", isAutoMode);
        }
        return;
    }

    // All other commands below require manual mode
    if (isAutoMode)
        return;

    if (dataPath == "/relay1")
        setRelay(RELAY1, state);
    else if (dataPath == "/relay2")
        setRelay(RELAY2, state);
}

void startFirebaseStream()
{
    uid = app.getUid().c_str();
    if (uid.isEmpty())
        return; // Exit if UID is invalid

    // Setup database paths
    baseUserPath = "UsersData/" + uid;
    logsPath = baseUserPath + "/logs";
    cmdPath = baseUserPath + "/cmd";
    statePath = baseUserPath + "/state";
    
    // Initialize state values in Firebase
    Database.set<bool>(aClient, statePath + "/relay1", digitalRead(RELAY1) == LOW);
    Database.set<bool>(aClient, statePath + "/relay2", digitalRead(RELAY2) == LOW);

    // Configure and start the command stream
    streamClient.setSSEFilters("get,put,patch,keep-alive,cancel,auth_revoked");
    Database.get(streamClient, cmdPath, handleFirebaseCommands, true, "streamCmd");

    streamStarted = true; // Set flag
}

// ============================================================================
// 5. SYSTEM INITIALIZATION
// ============================================================================

void startSystem()
{
    // LCD Init
    lcd.init();
    lcd.backlight();

    lcd.clear();
    lcd.print("Initializing...");

    // GPIO Pins
    pinMode(PIN_TRIG, OUTPUT);
    pinMode(PIN_ECHO, INPUT);
    pinMode(RELAY1, OUTPUT);
    pinMode(RELAY2, OUTPUT);
    setRelay(RELAY1, false);
    setRelay(RELAY2, false);
    pinMode(PIN_LDR, INPUT);

    // RTC & DS18B20
    rtc.begin();
    ds.begin();

    // Modbus (SHT20)
    pinMode(MODBUS_EN_PIN, OUTPUT);
    digitalWrite(MODBUS_EN_PIN, LOW);
    Serial2.begin(MODBUS_SERIAL_BAUD, MODBUS_PARITY, MODBUS_RO_PIN, MODBUS_DI_PIN);
    Serial2.setTimeout(1000);
    modbus.begin(MODBUS_SLAVE_ID, Serial2);
    modbus.preTransmission(modbusPreTransmission);
    modbus.postTransmission(modbusPostTransmission);

    delay(INITIALIZE_INTERVAL);
}

void initWiFi()
{
    lcd.clear();
    lcd.print("Connecting to");
    lcd.setCursor(0, 1);
    lcd.print(WIFI_SSID);
    delay(INITIALIZE_INTERVAL); // show message

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    unsigned long startAttemptTime = millis();
    bool wifiConnected = false;

    // Connection loop with timeout
    while (millis() - startAttemptTime < (WIFI_CONNECT_TIMEOUT - INITIALIZE_INTERVAL))
    {
        if (WiFi.status() == WL_CONNECTED)
        {
            lcd.clear();
            lcd.print("Success Connect");
            lcd.setCursor(0, 1);
            lcd.print("to " + String(WIFI_SSID));

            wifiConnected = true;
            delay(INITIALIZE_INTERVAL);
            break;
        }
        delay(100); // Small delay to avoid busy loop
    }

    // Handle connection failure
    if (!wifiConnected)
    {
        lcd.clear();
        lcd.print("Failed Connect");
        lcd.setCursor(0, 1);
        lcd.print("to " + String(WIFI_SSID));

        delay(INITIALIZE_INTERVAL);
    }
}

void initFirebaseServices()
{
    ssl_client.setInsecure();
    stream_ssl_client.setInsecure();

    initializeApp(aClient, app, getAuth(user_auth), nullptr, "authTask");
    app.getApp<RealtimeDatabase>(Database);
    Database.url(DATABASE_URL);
}

// ============================================================================
// 6. MAIN LOOP HANDLERS
// ============================================================================

void handleSensorUpdates(unsigned long now)
{
    if (now - lastSensorRead >= SENSOR_READ_INTERVAL)
    {
        lastSensorRead = now;
        updateSensorsAndLCD();
    }
}

void handleAutomation()
{
    if (isAutoMode)
        runAutomation();
}

void handleFirebaseConnection()
{
    // Firebase App Loop & Stream Management
    if (WiFi.status() == WL_CONNECTED)
    {
        app.loop();

        // Start stream if not already started
        if (!streamStarted && app.ready())
            startFirebaseStream();
    }
    else
    {
        streamStarted = false; // Reset flag on WiFi disconnect
    }
}

void handleLogUpload(unsigned long now)
{
    if (now - lastUpload >= LOG_UPDATE_INTERVAL)
    {
        lastUpload = now;

        // Upload data to Firebase
        if (app.ready() && !uid.isEmpty())
            uploadLogData();
    }
}

void handleStateUpload(unsigned long now)
{
    if (now - lastDashboardUpload >= STATE_UPDATE_INTERVAL)
    {
        lastDashboardUpload = now;

        if (app.ready() && !uid.isEmpty())
        {
            uploadStateData();
        }
    }
}

// ============================================================================
// 7. ESP32 SETUP & LOOP
// ============================================================================

void setup()
{
    // System Initialization
    startSystem();

    // Initialize WiFi connection
    initWiFi();

    // Initial Sensor Read & LCD Update
    updateSensorsAndLCD();

    // Initialize Firebase services
    initFirebaseServices();
}

void loop()
{
    unsigned long now = millis();

    // Offline Tasks
    handleSensorUpdates(now); // Sensor Reading & LCD Update
    handleAutomation();       // Automation Logic

    // Online Tasks
    handleFirebaseConnection(); // Firebase Connection Management
    handleStateUpload(now);     // Dashboard State Update
    // handleLogUpload(now);    // Log Data Upload

    delay(5); // Small delay for stability
}