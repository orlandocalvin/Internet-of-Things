#include <Wire.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_SSD1306.h>

const char* ssid = "TELLO-9C71B4";
const char* password = "";

const int btnLanding = 9;  // Button to toggle takeoff/landing
bool droneFly = false;  // Track if the drone is flying

WiFiUDP udp;
const char* TELLO_IP = "192.168.10.1";
const int TELLO_PORT = 8889;
const int LOCAL_PORT = 9000;

Adafruit_MPU6050 mpu;
Adafruit_SSD1306 display(128, 32, &Wire, -1);

// Moving average filter buffer
const int sampleCount = 10;
float accX_samples[sampleCount], accY_samples[sampleCount], accZ_samples[sampleCount];
int sampleIndex = 0;

// Variables to store filtered acceleration values
float accX_avg = 0, accY_avg = 0, accZ_avg = 0;

// Threshold & scaling factors
const float rollThreshold = 5.0;
const float pitchThreshold = 5.0;
const int maxSpeed = 50;

// Function to read MPU6050 data with a moving average filter
void getFilteredData() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  // Store data in moving average buffer
  accX_samples[sampleIndex] = a.acceleration.x;
  accY_samples[sampleIndex] = a.acceleration.y;
  accZ_samples[sampleIndex] = a.acceleration.z;

  sampleIndex = (sampleIndex + 1) % sampleCount;

  // Compute average values
  accX_avg = accY_avg = accZ_avg = 0;
  for (int i = 0; i < sampleCount; i++) {
    accX_avg += accX_samples[i];
    accY_avg += accY_samples[i];
    accZ_avg += accZ_samples[i];
  }
  accX_avg /= sampleCount;
  accY_avg /= sampleCount;
  accZ_avg /= sampleCount;
}

// Function to send a command to the drone
bool sendCommand(const char* cmd) {
  udp.beginPacket(TELLO_IP, TELLO_PORT);
  udp.print(cmd);
  udp.endPacket();

  // Wait for a response from the drone
  char response[50] = {0};
  int responseLength = udp.parsePacket();
  if (responseLength) {
    udp.read(response, sizeof(response) - 1);
  } else {
    strcpy(response, "No response");
  }

  // Display command and response on OLED
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("CMD: ");
  display.println(cmd);
  display.setCursor(0, 10);
  display.print("RES: ");
  display.println(response);
  display.display();

  // Check if the response contains "ok"
  return strstr(response, "ok") != nullptr;
}

void setup() {
  Wire.begin();
  pinMode(btnLanding, INPUT_PULLUP);
  WiFi.begin(ssid, password);

  // Initialize OLED display
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    while (true);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  // Initialize MPU6050
  if (!mpu.begin()) {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("MPU6050 not detected!");
    display.display();
    while (true);
  }

  // Connect to Tello WiFi
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Connecting to Tello...");
  display.display();

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Connected to Tello!");
  display.display();

  udp.begin(LOCAL_PORT);

  // Try entering SDK mode up to 5 times
  bool connected = false;
  for (int i = 0; i < 5; i++) {
    if (sendCommand("command")) {
      connected = true;
      break;
    }
    delay(1000);
  }

  if (!connected) {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Failed to enter SDK mode!");
    display.display();
    while (true);
  }
}

void loop() {
  if (digitalRead(btnLanding) == LOW) {
    sendCommand(droneFly ? "land" : "takeoff");
    droneFly = !droneFly;
    delay(1000);
  }

  if (droneFly) {
    getFilteredData();

    // Compute roll & pitch in degrees
    float roll = atan2(accY_avg, sqrt(accX_avg * accX_avg + accZ_avg * accZ_avg)) * 180.0 / M_PI;
    float pitch = atan2(-accX_avg, accZ_avg) * 180.0 / M_PI;

    // Convert to drone speed
    int rollSpeed = (abs(roll) > rollThreshold) ? constrain(roll * 2, -maxSpeed, maxSpeed) : 0;
    int pitchSpeed = (abs(pitch) > pitchThreshold) ? constrain(pitch * 2, -maxSpeed, maxSpeed) : 0;

    // Send movement command to drone
    String rcCommand = "rc " + String(rollSpeed) + " " + String(pitchSpeed) + " 0 0";
    sendCommand(rcCommand.c_str());
  }

  delay(15);
}