#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <FirebaseClient.h>

// WiFi credentials
const char* ssid = "SUPER-ORCA";
const char* password = "zxcvbnmv";

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

// Timer for sending data every 10 seconds
unsigned long previousTime = 0;
const unsigned long interval = 10000; // 10 seconds

// Data variables
int intValue = 0;
float floatValue = 0.01;
String stringValue = "";

void connectWiFi() {
  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println();
  Serial.print("Connected! IP: ");
  Serial.println(WiFi.localIP());
}

void setupFirebase() {
  Serial.println("Setting up Firebase...");
  
  // Configure SSL
  ssl_client.setInsecure(); // Skip certificate check (for learning only)
  
  // Initialize Firebase
  initializeApp(aClient, app, getAuth(user_auth), handleFirebaseResponse, "auth");
  app.getApp<RealtimeDatabase>(Database);
  Database.url(DATABASE_URL);
  
  Serial.println("Firebase setup complete");
}

void sendData() {
  Serial.println("\nSending data to Firebase...");

  // Send a string
  unsigned long seconds = millis() / 1000; // convert to seconds
  stringValue = String(seconds) + "s";
  Database.set<String>(aClient, "/test/string", stringValue, handleFirebaseResponse, "send_string");

  // Send an int
  Database.set<int>(aClient, "/test/int", intValue, handleFirebaseResponse, "send_int");
  intValue++; // increment int

  // Send a float
  floatValue = 0.01 + random(0, 100); // random float
  Database.set<float>(aClient, "/test/float", floatValue, handleFirebaseResponse, "send_float");

  Serial.println("Data sent!");
}

void handleFirebaseResponse(AsyncResult &result) {
  // Check if there's an error
  if (result.isError()) {
    Serial.println("Firebase Error: " + result.error().message());
    return;
  }
  
  // Print success message
  if (result.available()) {
    Serial.println("Firebase Response: " + String(result.c_str()));
  }
}

void setup() {
  Serial.begin(115200);
  
  connectWiFi(); // Connect to WiFi
  setupFirebase(); // Setup Firebase
  
  Serial.println("Setup complete!");
}

void loop() {
  app.loop(); // Keep Firebase running
  
  // Check if connected to Firebase
  if (app.ready()) {
    // Check if 10 seconds have passed
    if (millis() - previousTime >= interval) {
      sendData();
      previousTime = millis(); // Reset timer
    }
  }
}