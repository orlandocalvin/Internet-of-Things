# Internet-of-Things

My IoT learning journey with Arduino, ESP32/ESP8266, C++ and PlatformIO, plus the projects that came out of it: a hydroponic system, a micro-hydro (PLTMH) monitor, a Wi-Fi RC car and more.

## Contents

### Learning journey (`my_learning_journey/`)

| Folder | What it is | Stack |
|---|---|---|
| `bab1/` | Arduino basics: digital/analog I/O, loops, arrays, functions, `millis`, EEPROM, servo, LCD, keypad, relay, LDR, ultrasonic sensor, a small security system | Arduino (C++) |
| `bab2 - starter kit arduino/` | Starter kit exercises: LEDs, switches, I2C LCD, DHT11 thermometer, HC-SR04, PIR, LDR, RGB LED, IR sensor, servo, keypad + LCD | Arduino (C++) |
| `bab3/` | Interrupts, PWM, IR remote, RTC, I2C scanner, Blynk, SPIFFS web pages, controlling an ESP32 from a web page, sending sensor data to a PHP/MySQL API | Arduino, ESP32, PHP |
| `bab4/` | RFID, MPU6050 roll/pitch, OLED + I2C keypad, ESP-NOW, Web Bluetooth (BLE), web servers (sync and async, SPIFFS), Telegram bots, WiFiMulti/WiFiManager, Cheap Yellow Display (TFT_eSPI, LVGL) | ESP32 |
| `bab5/` | OTA updates (ArduinoOTA, ElegantOTA, Telnet), UDP and JSON over UDP, MQTT (public broker, HiveMQ Cloud, WebSocket and three.js pages), NTP-synced RTC, Firebase (ESP32 firmware + web apps with Realtime Database and Auth) | ESP32, PlatformIO, Firebase, JS |
| `bab6 - bpvp x edutic/` | Industrial protocols course: Modbus RTU and Modbus TCP, MQTT publishers/subscribers with DHT11, relay, LCD and touch pins | ESP32 |
| `learn_cpp/` | Console C++ basics: variables, constants, naming, math expressions, console I/O, standard library, random numbers | C++ |
| `platfromio/` | First PlatformIO test project (blink on an ESP32 dev board) | PlatformIO, ESP32 |

### Projects (`projects/`)

| Folder | What it is | Stack |
|---|---|---|
| `Atama Climate/` | Weather station: rain gauge, anemometer, BH1750 light, DHT22, INA219 power monitoring, RTC and LCD, posting JSON over a SIM7600 4G modem (or Wi-Fi in the tests). Includes single-sensor tests and Fritzing parts | ESP32, SIM7600 |
| `Hydroponic System/` | Ebb and flow hydroponics controller with sensors, relays, RTC schedule and LCD, plus a dashboard for auto/manual control | ESP32 (PlatformIO), Firebase |
| `PLTMH/` | Micro-hydro power monitor: INA219 voltage/current/power and Hall-sensor RPM sent to a Firebase dashboard. Includes 3D printable parts (STL) | ESP32, Firebase |
| `Ryze Tello Drone/` | Tello drone control: Python scripts (DJITelloPy) and ESP32 sketches over UDP, including autonomous flight and MPU6050 gesture control | Python, ESP32 |
| `Smartdam/` | Dam monitor: water level, temperature/humidity and rain sent to a PHP/MySQL web app with history, CSV export and remote gate (servo) control | ESP32, PHP, MySQL |
| `WiFi RC Car/` | RC car with a web control page served from LittleFS, plus an MPU6050 tilt controller that sends commands over UDP | ESP8266, ESP32 |
| `rosus/` | MPU6050 motion sensors streaming orientation over UDP to Python scripts that drive Dynamixel servos, plus a web page showing live sensor data | ESP32, Python |

## Running a sketch

Open the `.ino` file in the Arduino IDE, install the libraries it includes, select your board and upload. Folders with a `platformio.ini` and `src/main.cpp` are PlatformIO projects: open them in VS Code with the PlatformIO extension, or run `pio run -t upload`.

Credentials are not in the code. Sketches that need them include a gitignored `secrets.h`: copy `secrets.example.h` to `secrets.h` in the same folder and fill in your own Wi-Fi, Firebase, MQTT or bot values. The PHP APIs work the same way with `config.php`: copy `config.example.php` to `config.php` and fill in your database settings.

Sketches with a `data/` folder serve web pages from SPIFFS or LittleFS, so upload that folder to the board's filesystem as well. The Firebase web apps (`public/` folders) are deployed with the Firebase CLI (`firebase deploy`) after pointing their Firebase config at your own project.
