import dynamixel_sdk as dxl  # Import Dynamixel SDK

# Parameter koneksi
DEVICENAME = 'COM4'         # Port serial U2D2
BAUDRATE = 57600            # Baudrate
PROTOCOL_VERSION = 1.0      # AX-12A menggunakan protocol 1.0
DXL_ID_1 = 1                # ID servo pertama
DXL_ID_2 = 2                # ID servo kedua

# Alamat Control Table
ADDR_MX_TORQUE_ENABLE = 24        
ADDR_MX_GOAL_POSITION = 30        
ADDR_MX_PRESENT_POSITION = 36     
TORQUE_ENABLE = 1                 
TORQUE_DISABLE = 0                

# Inisialisasi PortHandler dan PacketHandler
portHandler = dxl.PortHandler(DEVICENAME)
packetHandler = dxl.PacketHandler(PROTOCOL_VERSION)

# Buka port
portHandler.openPort()
portHandler.setBaudRate(BAUDRATE)

# Fungsi untuk mengaktifkan torque pada servo
def enable_torque(dxl_id):
    packetHandler.write1ByteTxRx(portHandler, dxl_id, ADDR_MX_TORQUE_ENABLE, TORQUE_ENABLE)

# Fungsi untuk mengatur posisi target pada servo
def set_goal_position(dxl_id, position):
    packetHandler.write2ByteTxRx(portHandler, dxl_id, ADDR_MX_GOAL_POSITION, position)

# Fungsi untuk membaca posisi saat ini pada servo
def read_position(dxl_id):
    position, _, _ = packetHandler.read2ByteTxRx(portHandler, dxl_id, ADDR_MX_PRESENT_POSITION)
    return position

# Aktifkan torque pada kedua servo
enable_torque(DXL_ID_1)
enable_torque(DXL_ID_2)

try:
    while True:
        # Meminta input posisi untuk Servo 1
        pos1_input = int(input("Masukkan posisi untuk Servo 1 (0-1023): "))
        if 0 <= pos1_input <= 1023:
            set_goal_position(DXL_ID_1, pos1_input)
            print(f"Posisi Servo 1 diatur ke: {pos1_input}")
        else:
            print("Nilai tidak valid. Masukkan angka antara 0 hingga 1023.")

        # Meminta input posisi untuk Servo 2
        pos2_input = int(input("Masukkan posisi untuk Servo 2 (0-1023): "))
        if 0 <= pos2_input <= 1023:
            set_goal_position(DXL_ID_2, pos2_input)
            print(f"Posisi Servo 2 diatur ke: {pos2_input}")
        else:
            print("Nilai tidak valid. Masukkan angka antara 0 hingga 1023.")

        # Menampilkan posisi saat ini dari kedua servo
        pos1 = read_position(DXL_ID_1)
        pos2 = read_position(DXL_ID_2)
        print(f"Posisi Saat Ini - Servo 1: {pos1}, Servo 2: {pos2}\n")

except KeyboardInterrupt:
    print("Pengaturan posisi dihentikan oleh pengguna.")

# Nonaktifkan torque setelah selesai
packetHandler.write1ByteTxRx(portHandler, DXL_ID_1, ADDR_MX_TORQUE_ENABLE, TORQUE_DISABLE)
packetHandler.write1ByteTxRx(portHandler, DXL_ID_2, ADDR_MX_TORQUE_ENABLE, TORQUE_DISABLE)

# Tutup port
portHandler.closePort()
