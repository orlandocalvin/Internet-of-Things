#include <ESP8266WiFi.h> 
#include <ESP8266WebServer.h> 

// connections & values for drive Motors
const int RIGHT_GAS = D1, LEFT_GAS = D2, RIGHT_DIR = D3, LEFT_DIR = D4;
const int OFF = 0, ON = 1, FORWARD = 0, BACKWARD = 1;

const int buzPin = D5;
const int ledPin = D8;

String command;
ESP8266WebServer server(80);

// set AP variable
const char* ssid = "REDACTED";
const char* password = "REDACTED";
const int channel = 6, max_connections = 1;
bool hidden = false;

void setup(){
  Serial.begin(115200);    
  
  // set buzzer & led
  pinMode(buzPin, OUTPUT);      
  pinMode(ledPin, OUTPUT);      
  digitalWrite(buzPin, LOW);
  digitalWrite(ledPin, LOW);
    
  // Set all the motor control pins to outputs
  pinMode(RIGHT_GAS, OUTPUT);
  pinMode(LEFT_GAS, OUTPUT);
  pinMode(RIGHT_DIR, OUTPUT);
  pinMode(LEFT_DIR, OUTPUT);
  
  // Turn off motors - Initial state
  digitalWrite(RIGHT_DIR, LOW);
  digitalWrite(LEFT_DIR, LOW);
  digitalWrite(RIGHT_GAS, 0);
  digitalWrite(LEFT_GAS, 0);

  // setup AP
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password, channel, hidden, max_connections);
  Serial.println("AP IP address: "+ WiFi.softAPIP().toString());
  delay(3000);
  
  // setup webserver
  server.on("/", HTTP_handleRoot);       
  server.onNotFound(HTTP_handleRoot);    
  server.begin();                         
}

void loop() {
  server.handleClient();

  command = server.arg("State");          
  if (command == "F") Forward();          
  else if (command == "B") Backward();
  else if (command == "R") TurnRight();
  else if (command == "L") TurnLeft();
  else if (command == "S") Stop();
  else if (command == "V") BeepHorn();
  else if (command == "W") TurnLightOn();
  else if (command == "w") TurnLightOff();
}

void HTTP_handleRoot(void){
  server.send(200, "text/html", "");       
  
  if(server.hasArg("State")){
     Serial.println(server.arg("State"));
  }
}

void handleNotFound(){
  server.send(404, "text/plain", "404: Not found"); 
}

void Throttle(int gas) {
  digitalWrite(RIGHT_GAS, gas);
  digitalWrite(LEFT_GAS, gas);
}

// function to move forward
void Forward(){ 
  digitalWrite(RIGHT_DIR, FORWARD);
  digitalWrite(LEFT_DIR, FORWARD);
  Throttle(ON);
}

// function to move backward
void Backward(){
  digitalWrite(RIGHT_DIR, BACKWARD);
  digitalWrite(LEFT_DIR, BACKWARD);
  Throttle(ON);
}

// function to turn right
void TurnRight(){
  digitalWrite(RIGHT_DIR, BACKWARD);
  digitalWrite(LEFT_DIR, FORWARD);
  Throttle(ON);
}

// function to turn left
void TurnLeft(){
  digitalWrite(RIGHT_DIR, FORWARD);
  digitalWrite(LEFT_DIR, BACKWARD);
  Throttle(ON);
}

// function to stop motors
void Stop(){  
  digitalWrite(RIGHT_DIR, FORWARD);
  digitalWrite(LEFT_DIR, FORWARD);
  Throttle(OFF);
}

// function to beep a buzzer
void BeepHorn(){
  digitalWrite(buzPin, HIGH);
  delay(150);
  digitalWrite(buzPin, LOW);
  delay(80);
}

// function to turn on LED
void TurnLightOn(){
  digitalWrite(ledPin, HIGH);
}

// function to turn off LED
void TurnLightOff(){
  digitalWrite(ledPin, LOW);
}