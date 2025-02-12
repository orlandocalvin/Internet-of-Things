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

# Tunggu 5 detik di udara
time.sleep(5)

# Mendarat
tello.land()

# Matikan koneksi
tello.end()