import socket
import json
import dynamixel_sdk as dxl

# Konfigurasi UDP
UDP_IP = "0.0.0.0"
UDP_PORT = 4321

# Konfigurasi Dynamixel
DEVICENAME = 'COM4'
BAUDRATE = 57600
PROTOCOL_VERSION = 1.0
ADDR_MX_TORQUE_ENABLE = 24
ADDR_MX_GOAL_POSITION = 30
ADDR_MX_PRESENT_POSITION = 36
TORQUE_ENABLE = 1
TORQUE_DISABLE = 0

# Alokasi ID Servo untuk Setiap Sender
sender_to_servo = {
    "192.168.137.102": {"pitch": 2, "roll": 4},  # Servo ID 2 -> pitch, Servo ID 4 -> roll
    "192.168.137.103": {"pitch": 3},
    "192.168.137.106": {"pitch": 6},
    "192.168.137.107": {"pitch": 7, "roll": 5}  # Servo ID 7 -> pitch, Servo ID 5 -> roll
}

# Fungsi Konversi Derajat ke Posisi Servo
# def deg_to_position(pitch):
#     position = int((pitch + 90) / 180 * 1023)
#     return max(0, min(position, 1023))

def right_shoulder(pitch):
    position = int((pitch / 150) * (1023 - 512) + 512)
    return max(512, min(position, 1023))

def left_shoulder(pitch):
    position = int((pitch / 150) * (1023 - 512) + 512)
    return max(0, min(position, 512))

def arm(pitch, angle_range, position_range):
    min_angle, max_angle = angle_range
    min_position, max_position = position_range

    position = int((pitch - min_angle) / (max_angle - min_angle) * (max_position - min_position) + min_position)

    return max(min_position, min(position, max_position))

# Inisialisasi UDP Socket
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind((UDP_IP, UDP_PORT))
print(f"Listening for UDP JSON packets on IP {UDP_IP}, port {UDP_PORT}...")

# Inisialisasi PortHandler & PacketHandler Dynamixel
portHandler = dxl.PortHandler(DEVICENAME)
packetHandler = dxl.PacketHandler(PROTOCOL_VERSION)
portHandler.openPort()
portHandler.setBaudRate(BAUDRATE)

# Mengaktifkan Torque untuk Semua Servo yang Terdaftar
servo_ids = {servo_id for mapping in sender_to_servo.values() for servo_id in mapping.values()}
for servo_id in servo_ids:
    dxl_comm_result, dxl_error = packetHandler.write1ByteTxRx(portHandler, servo_id, ADDR_MX_TORQUE_ENABLE, TORQUE_ENABLE)
    if dxl_comm_result != dxl.COMM_SUCCESS:
        print(f"Failed to enable torque for Servo ID {servo_id}. Error: {packetHandler.getTxRxResult(dxl_comm_result)}")
    else:
        print(f"Torque enabled for Servo ID {servo_id}")

# Loop Utama
try:
    right_upper_arm = 0
    left_upper_arm = 0

    while True:
        # Menerima data UDP
        data, addr = sock.recvfrom(1024)
        sender_ip = addr[0]  # Mendapatkan IP sender
        json_data = json.loads(data.decode())
        
        # Periksa apakah sender IP terdaftar dalam mapping
        if sender_ip in sender_to_servo:
            servo_mapping = sender_to_servo[sender_ip]  # Ambil mapping servo untuk IP tersebut
            
            # Tangani pitch jika tersedia
            if "pitch" in servo_mapping and "pitch" in json_data:
                servo_id = servo_mapping["pitch"]

                if sender_ip == "192.168.137.103":
                    right_upper_arm = json_data['pitch']
                    dxl_goal_position = arm(right_upper_arm, (90, -90), (204, 820))
                elif sender_ip == "192.168.137.102":
                    dxl_goal_position = arm(json_data['pitch'], (right_upper_arm, right_upper_arm - 150), (512, 1023))
                elif sender_ip == "192.168.137.106":
                    left_upper_arm = json_data['pitch']
                    dxl_goal_position = arm(left_upper_arm, (-90, 90), (204, 820))
                elif sender_ip == "192.168.137.107":
                    dxl_goal_position = arm(json_data['pitch'], ((left_upper_arm - 150), left_upper_arm), (0, 512))
                else:
                    dxl_goal_position = 0
                
                # Kirim posisi ke servo pitch yang sesuai
                dxl_comm_result, dxl_error = packetHandler.write2ByteTxRx(
                    portHandler, servo_id, ADDR_MX_GOAL_POSITION, dxl_goal_position
                )
                # if dxl_comm_result != dxl.COMM_SUCCESS:
                #     print(f"Error setting pitch for Servo ID {servo_id}: {packetHandler.getTxRxResult(dxl_comm_result)}")
                # else:
                #     print(f"Sender IP: {sender_ip}, Servo ID: {servo_id}, Pitch Goal Position: {dxl_goal_position}")
            
            # Tangani roll jika tersedia
            if "roll" in servo_mapping and "roll" in json_data:
                servo_id = servo_mapping["roll"]

                if sender_ip == "192.168.137.102":
                    dxl_goal_position = right_shoulder(json_data['roll'])
                elif sender_ip == "192.168.137.107":
                    dxl_goal_position = left_shoulder(json_data['roll'])
                else:
                    dxl_goal_position = 0

                # Kirim posisi ke servo roll yang sesuai
                dxl_comm_result, dxl_error = packetHandler.write2ByteTxRx(
                    portHandler, servo_id, ADDR_MX_GOAL_POSITION, dxl_goal_position
                )
                # if dxl_comm_result != dxl.COMM_SUCCESS:
                #     print(f"Error setting roll for Servo ID {servo_id}: {packetHandler.getTxRxResult(dxl_comm_result)}")
                # else:
                #     print(f"Sender IP: {sender_ip}, Servo ID: {servo_id}, Roll Goal Position: {dxl_goal_position}")
        
        else:
            print(f"Sender IP {sender_ip} tidak terdaftar!")

except KeyboardInterrupt:
    print("\nKeyboard Interrupt detected. Shutting down...")

    # Nonaktifkan torque untuk semua servo
    for servo_id in servo_ids:
        try:
            dxl_comm_result, dxl_error = packetHandler.write1ByteTxRx(
                portHandler, servo_id, ADDR_MX_TORQUE_ENABLE, TORQUE_DISABLE
            )
            if dxl_comm_result != dxl.COMM_SUCCESS:
                print(f"Error disabling torque for Servo ID {servo_id}: {packetHandler.getTxRxResult(dxl_comm_result)}")
            else:
                print(f"Torque disabled for Servo ID {servo_id}")
        except Exception as e:
            print(f"Exception during torque disable for Servo ID {servo_id}: {e}")

    # Tutup Port dan Socket
    portHandler.closePort()
    sock.close()
    print("Program terminated.")
