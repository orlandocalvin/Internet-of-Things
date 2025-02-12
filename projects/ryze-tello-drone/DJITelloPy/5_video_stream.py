from djitellopy import Tello
import cv2

# Buat objek drone
tello = Tello()

# Hubungkan ke drone
tello.connect()

# Mulai video streaming
tello.streamon()

while True:
    # Ambil frame dari drone
    frame = tello.get_frame_read().frame

    # Ubah ukuran frame (opsional)
    frame = cv2.resize(frame, (640, 480))

    # Tampilkan frame
    cv2.imshow("Tello Video Stream", frame)

    # Tekan 'q' untuk keluar
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

# Matikan video streaming
tello.streamoff()
cv2.destroyAllWindows()