from djitellopy import Tello

tello = Tello()

# Coba connect ke drone
tello.connect()

# Cek status baterai
print(f"Battery: {tello.get_battery()}%")

# Disconnect
tello.end()