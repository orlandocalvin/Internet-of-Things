# Ryze Tello Drone

Koleksi project untuk mengendalikan drone DJI Tello (Ryze Tello) menggunakan Python (DJITelloPy) dan Arduino.

## Struktur Folder

```
Ryze Tello Drone/
├── arduino/
│   ├── 1_wifi_connection/
│   │   └── 1_wifi_connection.ino           # Koneksi ESP32 ke Wi-Fi Tello
│   ├── 2_udp_communication/
│   │   └── 2_udp_communication.ino         # Komunikasi UDP & pengecekan baterai
│   ├── 3_autonomous_oled/
│   │   └── 3_autonomous_oled.ino           # Terbang otomatis + OLED feedback
│   └── 4_gesture_control_imu/
│       └── 4_gesture_control_imu.ino       # Kontrol drone via IMU (MPU6050)
└── python/
    ├── 1_connection_test.py                 # Tes koneksi dasar
    ├── 2_takeoff_land.py                    # Lepas landas dan mendarat
    ├── 3_move_drone.py                      # Gerak maju/mundur/kiri/kanan
    ├── 4_speed_height.py                    # Kontrol kecepatan & ketinggian
    ├── 5_video_stream.py                    # Streaming video
    ├── 6_take_photo.py                      # Mengambil foto
    ├── 7_record_video.py                    # Merekam video
    ├── 8_keyboard_control.py                # Kontrol real-time via keyboard
    └── 9_autonomous_square.py               # Terbang otonom membentuk kotak
```

## Persiapan

### Python Scripts
```bash
pip install djitellopy opencv-python
```

### Arduino Sketches
- ESP32-C3 (atau ESP32 lain)
- Library: `Adafruit_GFX`, `Adafruit_SSD1306`, `Adafruit_MPU6050` (untuk sketch `4_gesture_control_imu`)

Terhubung ke Wi-Fi drone (SSID format: `TELLO-XXXXXX`), lalu upload sketch.

## Catatan
- Drone harus dalam mode SDK (`command` perlu dikirim pertama kali)
- Port UDP default: `8889` (drone), `9000` (lokal)
