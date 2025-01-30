import dynamixel_sdk as dxl  # Import Dynamixel SDK

# Parameter koneksi
DEVICENAME = 'COM4'         # Ganti dengan port serial U2D2 di komputer Anda
BAUDRATE = 57600            # Baudrate default AX-12A
PROTOCOL_VERSION = 1.0      # AX-12A menggunakan protocol 1.0
DXL_ID = 1                  # ID dari AX-12A, default biasanya 1

# Alokasi Control Table Address untuk AX-12A
ADDR_MX_TORQUE_ENABLE = 24        # Alamat untuk mengaktifkan Torque
ADDR_MX_GOAL_POSITION = 30        # Alamat untuk posisi target
ADDR_MX_PRESENT_POSITION = 36     # Alamat untuk posisi saat ini
TORQUE_ENABLE = 1                 # Nilai untuk mengaktifkan torque
TORQUE_DISABLE = 0                # Nilai untuk menonaktifkan torque
DXL_MINIMUM_POSITION_VALUE = 0    # Nilai minimum posisi (0 derajat)
DXL_MAXIMUM_POSITION_VALUE = 1023 # Nilai maksimum posisi (300 derajat)

# Inisialisasi PortHandler dan PacketHandler
portHandler = dxl.PortHandler(DEVICENAME)
packetHandler = dxl.PacketHandler(PROTOCOL_VERSION)

# Buka port
if portHandler.openPort():
    print("Port berhasil dibuka")
else:
    print("Gagal membuka port")
    quit()

# Atur baudrate
if portHandler.setBaudRate(BAUDRATE):
    print("Baudrate berhasil diatur")
else:
    print("Gagal mengatur baudrate")
    quit()

# Mengaktifkan torque pada Dynamixel
dxl_comm_result, dxl_error = packetHandler.write1ByteTxRx(portHandler, DXL_ID, ADDR_MX_TORQUE_ENABLE, TORQUE_ENABLE)
if dxl_comm_result != dxl.COMM_SUCCESS:
    print(f"Error: {packetHandler.getTxRxResult(dxl_comm_result)}")
elif dxl_error != 0:
    print(f"Error: {packetHandler.getRxPacketError(dxl_error)}")
else:
    print("Torque berhasil diaktifkan")

# Loop untuk menerima input posisi secara real-time
while True:
    # Menerima input posisi dari pengguna
    try:
        dxl_goal_position = int(input("Masukkan posisi target (0-1023): "))
        
        # Pastikan posisi berada dalam rentang yang valid
        if dxl_goal_position < 0 or dxl_goal_position > 1023:
            print("Posisi tidak valid. Silakan masukkan nilai antara 0 dan 1023.")
            continue

        # Mengatur posisi target
        dxl_comm_result, dxl_error = packetHandler.write2ByteTxRx(portHandler, DXL_ID, ADDR_MX_GOAL_POSITION, dxl_goal_position)
        if dxl_comm_result != dxl.COMM_SUCCESS:
            print(f"Error: {packetHandler.getTxRxResult(dxl_comm_result)}")
        elif dxl_error != 0:
            print(f"Error: {packetHandler.getRxPacketError(dxl_error)}")
        else:
            print(f"Posisi target diatur ke {dxl_goal_position}")

    except ValueError:
        print("Masukkan angka yang valid!")

    # Membaca posisi saat ini setelah perintah diberikan
    dxl_present_position, dxl_comm_result, dxl_error = packetHandler.read2ByteTxRx(portHandler, DXL_ID, ADDR_MX_PRESENT_POSITION)
    if dxl_comm_result != dxl.COMM_SUCCESS:
        print(f"Error: {packetHandler.getTxRxResult(dxl_comm_result)}")
    elif dxl_error != 0:
        print(f"Error: {packetHandler.getRxPacketError(dxl_error)}")
    else:
        print(f"Posisi saat ini: {dxl_present_position}")
