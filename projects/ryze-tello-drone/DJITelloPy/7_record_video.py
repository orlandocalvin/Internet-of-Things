from djitellopy import Tello
import cv2

# Buat objek drone
tello = Tello()

# Hubungkan ke drone
tello.connect()

# Mulai video streaming
tello.streamon()

# Buat objek VideoWriter untuk menyimpan video
fourcc = cv2.VideoWriter_fourcc(*"XVID")
video_writer = cv2.VideoWriter("tello_video.avi", fourcc, 20.0, (640, 480))

print("Merekam video... Tekan 'q' untuk berhenti.")

while True:
    frame = tello.get_frame_read().frame
    frame = cv2.resize(frame, (640, 480))

    # Simpan frame ke video
    video_writer.write(frame)

    # Tampilkan video di layar
    cv2.imshow("Tello Video Stream", frame)

    # Tekan 'q' untuk keluar
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

# Matikan video streaming
tello.streamoff()
video_writer.release()
cv2.destroyAllWindows()
print("Video berhasil disimpan sebagai 'tello_video.avi'")