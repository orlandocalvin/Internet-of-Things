from djitellopy import Tello
import time

# Buat objek drone
tello = Tello()

# Hubungkan ke drone
tello.connect()

# Cek status baterai
print(f"Battery: {tello.get_battery()}%")

# Atur kecepatan (dalam cm/s, max 100)
tello.set_speed(50)
print(f"Speed set to: {tello.get_speed()} cm/s")

# Lepas landas
tello.takeoff()

# Naik 50 cm
tello.move_up(50)
time.sleep(2)

# Turun 50 cm
tello.move_down(50)
time.sleep(2)

# Maju dengan kecepatan yang diatur sebelumnya
tello.move_forward(100)
time.sleep(2)

# Mendarat
tello.land()

# Matikan koneksi
tello.end()