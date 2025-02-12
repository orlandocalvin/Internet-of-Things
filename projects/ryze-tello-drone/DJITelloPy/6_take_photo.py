from djitellopy import Tello
import cv2

# Buat objek drone
tello = Tello()

# Hubungkan ke drone
tello.connect()

# Mulai video streaming
tello.streamon()

# Ambil frame dari drone
frame = tello.get_frame_read().frame

# Simpan gambar
cv2.imwrite("tello_photo.jpg", frame)
print("Foto berhasil disimpan sebagai 'tello_photo.jpg'")

# Matikan video streaming
tello.streamoff()