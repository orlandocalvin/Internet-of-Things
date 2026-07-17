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

# Gerakan dasar
tello.move_forward(50)  # Maju 50 cm
time.sleep(2)

tello.move_back(50)  # Mundur 50 cm
time.sleep(2)

tello.move_left(50)  # Ke kiri 50 cm
time.sleep(2)

tello.move_right(50)  # Ke kanan 50 cm
time.sleep(2)

tello.rotate_clockwise(90)  # Putar searah jarum jam 90 derajat
time.sleep(2)

tello.rotate_counter_clockwise(90)  # Putar berlawanan jarum jam 90 derajat
time.sleep(2)

# Mendarat
tello.land()

# Matikan koneksi
tello.end()