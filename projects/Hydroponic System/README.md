# Hydroponic System (Project Mandiri)

Proyek mandiri sistem hidroponik terintegrasi berbasis IoT yang menggabungkan ESP32 (firmware ebb & flow) dan Web App dashboard (Firebase).

## Struktur Folder

```
Hydroponic System/
├── esp32/                              # Firmware ESP32 (PlatformIO)
│   ├── include/                        # Header files
│   ├── lib/                            # Library internal
│   ├── src/
│   │   ├── main.cpp                    # Kode utama (sensors, relay, Firebase)
│   │   ├── secrets.example.h           # Contoh kredensial (salin ke secrets.h)
│   │   └── secrets.h                   # Kredensial asli (tidak di-commit)
│   ├── test/                           # Unit test
│   ├── .vscode/                        # Konfigurasi VS Code (PlatformIO)
│   ├── platformio.ini                  # Konfigurasi PlatformIO
│   └── .gitignore
└── web_app/                            # Dashboard Web (Firebase Hosting)
    ├── public/
    │   ├── index.html                  # Halaman utama
    │   ├── scripts/                    # auth.js, index.js
    │   └── style.css
    ├── .firebase/
    ├── database.rules.json             # Aturan keamanan Realtime DB
    ├── firebase.json                   # Konfigurasi Firebase Hosting
    ├── .firebaserc
    └── .gitignore
```

## Komponen Sistem
1. **Sensor Suhu Nutrisi** (DS18B20) — memantau suhu larutan nutrisi.
2. **Sensor Level Nutrisi** (HC-SR04) — mengukur ketinggian air/larutan.
3. **Sensor Suhu & Kelembaban Udara** (SHT20 via Modbus RTU RS485).
4. **Sensor Cahaya** (LDR) — mengontrol lampu grow light.
5. **RTC (DS3231)** — penjadwalan pompa sirkulasi otomatis.
6. **Relay** — mengontrol pompa sirkulasi dan lampu grow light.
7. **LCD I2C 16x2** — menampilkan informasi sensor secara lokal.
8. **Firebase Realtime DB** — menyimpan data log & state, menerima perintah kontrol (auto/manual mode).

## Cara Menjalankan
### Firmware ESP32
Salin `src/secrets.example.h` menjadi `src/secrets.h`, lalu isi SSID/password Wi-Fi serta API key, URL database, email dan password Firebase milikmu.

```bash
cd esp32
pio run            # Build
pio run -t upload  # Flash ke board
```
> Atur `DEMO_MODE` di `src/main.cpp` (1 = demo cepat, 0 = kasus nyata).

### Web App
```bash
cd web_app
firebase deploy
```

## Catatan Keamanan
Kredensial Wi-Fi dan Firebase tidak ada di kode. Firmware membacanya dari `src/secrets.h`, yang diabaikan git (`.gitignore`); yang di-commit hanya `src/secrets.example.h`. Konfigurasi Firebase di `web_app/public` memang dirancang publik, jadi keamanan data diatur lewat `database.rules.json`.
