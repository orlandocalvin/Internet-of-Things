from djitellopy import Tello
import cv2
import time
from pynput import keyboard

# Buat objek drone
tello = Tello()

# Hubungkan ke drone
tello.connect()

# Mulai video streaming
tello.streamon()

# Fungsi untuk menangani input keyboard
def on_press(key):
    try:
        if key.char == "w":
            tello.move_forward(30)  # Maju
        elif key.char == "s":
            tello.move_back(30)  # Mundur
        elif key.char == "a":
            tello.move_left(30)  # Kiri
        elif key.char == "d":
            tello.move_right(30)  # Kanan
        elif key.char == "q":
            tello.rotate_counter_clockwise(30)  # Putar kiri
        elif key.char == "e":
            tello.rotate_clockwise(30)  # Putar kanan
        elif key.char == "r":
            tello.move_up(30)  # Naik
        elif key.char == "f":
            tello.move_down(30)  # Turun
        elif key.char == "t":
            tello.takeoff()  # Lepas landas
        elif key.char == "l":
            tello.land()  # Mendarat
        elif key.char == "x":
            tello.streamoff()  # Matikan video streaming
            tello.end()
            return False  # Keluar dari loop
    except AttributeError:
        pass  # Abaikan jika tombol bukan karakter

# Mulai pemantauan keyboard
listener = keyboard.Listener(on_press=on_press)
listener.start()

print("Gunakan tombol berikut untuk mengendalikan drone:")
print("W: Maju | S: Mundur | A: Kiri | D: Kanan")
print("Q: Putar kiri | E: Putar kanan")
print("R: Naik | F: Turun")
print("T: Takeoff | L: Landing")
print("X: Keluar")

while True:
    frame = tello.get_frame_read().frame
    frame = cv2.resize(frame, (640, 480))

    # Tampilkan video
    cv2.imshow("Tello Video Stream", frame)

    # Tekan 'q' untuk keluar
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cv2.destroyAllWindows()