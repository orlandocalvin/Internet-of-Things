from djitellopy import Tello
import time

# Buat objek drone
tello = Tello()

# Hubungkan ke drone
tello.connect()

# Cek status baterai
print(f"Battery: {tello.get_battery()}%")

# Lepas landas
tello.takeoff()
time.sleep(2)

# Terbang dalam pola persegi (4 sisi)
for _ in range(4):
    tello.move_forward(50)  # Maju 50 cm
    time.sleep(2)
    tello.rotate_clockwise(90)  # Putar 90 derajat searah jarum jam
    time.sleep(2)

# Mendarat
tello.land()

# Matikan koneksi
tello.end()