import socket
import json
import dynamixel_sdk as dxl

# UDP Connection
UDP_IP = "0.0.0.0"
UDP_PORT = 4321

# Dynamixel Connection
DEVICENAME = 'COM4'
BAUDRATE = 57600
PROTOCOL_VERSION = 1.0
DXL_ID_1 = 1
DXL_ID_2 = 2
DXL_ID_3 = 3
DXL_ID_4 = 4
DXL_ID_5 = 5
DXL_ID_6 = 6
DXL_ID_7 = 7
DXL_ID_8 = 8

# Control Table
ADDR_MX_TORQUE_ENABLE = 24
ADDR_MX_GOAL_POSITION = 30
ADDR_MX_PRESENT_POSITION = 36
TORQUE_ENABLE = 1
TORQUE_DISABLE = 0

# UDP Socket
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind((UDP_IP, UDP_PORT))
print(f"Listening for UDP JSON packets on IP {UDP_IP}, port {UDP_PORT}...")

# initialize PortHandler & PacketHandler
portHandler = dxl.PortHandler(DEVICENAME)
packetHandler = dxl.PacketHandler(PROTOCOL_VERSION)
portHandler.openPort()
portHandler.setBaudRate(BAUDRATE)

def enable_torque(dxl_id):
    packetHandler.write1ByteTxRx(portHandler, dxl_id, ADDR_MX_TORQUE_ENABLE, TORQUE_ENABLE)

def set_goal_position(dxl_id, position):
    packetHandler.write2ByteTxRx(portHandler, dxl_id, ADDR_MX_GOAL_POSITION, position)

# def shoulder(pitch):
#     position = int((pitch + 180) / 360 * 1023)
#     return max(0, min(position, 1023))

def right_shoulder(pitch):
    position = int((pitch / 150) * (1023 - 512) + 512)
    return max(512, min(position, 1023))

def left_shoulder(pitch):
    position = int((150 - pitch) / 150 * 512)
    return max(0, min(position, 512))

def arm(pitch, angle_range, position_range):
    min_angle, max_angle = angle_range
    min_position, max_position = position_range

    position = int((pitch - min_angle) / (max_angle - min_angle) * (max_position - min_position) + min_position)

    return max(min_position, min(position, max_position))


# Mengaktifkan torque
enable_torque(DXL_ID_4) #kanan
enable_torque(DXL_ID_3)
enable_torque(DXL_ID_2)

enable_torque(DXL_ID_5) #kiri
enable_torque(DXL_ID_6)
enable_torque(DXL_ID_7)

try:
    while True:
        data, addr = sock.recvfrom(1024)
        json_data = json.loads(data.decode())

        pitch1 = json_data.get("receiver", {}).get("pitch", 0)

        senders = json_data.get("senders", {}) # Pastikan "senders" ada dalam json_data
        pitch2 = senders.get("101", {}).get("pitch", 0)
        pitch3 = senders.get("102", {}).get("pitch", 0)
        pitch4 = senders.get("103", {}).get("pitch", 0)
        pitch5 = senders.get("104", {}).get("pitch", 0)
        pitch6 = senders.get("105", {}).get("pitch", 0)

        if 'senders' in json_data:
            pitch2 = json_data['senders'].get("101", {}).get("pitch", 0)
            pitch3 = json_data['senders'].get("102", {}).get("pitch", 0)
            pitch4 = json_data['senders'].get("103", {}).get("pitch", 0)
            pitch5 = json_data['senders'].get("104", {}).get("pitch", 0)
            pitch6 = json_data['senders'].get("105", {}).get("pitch", 0)
        else:
            pitch2 = pitch3 = pitch4 = pitch5 = pitch6 = 0

        # kanan
        right_upper_arm_angle = (90, -90)
        right_upper_arm_position = (204, 820)
        right_lower_arm_angle = (pitch1, (pitch1 - 150))
        right_lower_arm_position = (512, 1023)

        # kiri
        left_upper_arm_angle = (-90, 90)
        left_upper_arm_position = (204, 820)
        left_lower_arm_angle = ((pitch4 - 150), pitch4)
        left_lower_arm_position = (0, 512)

        # kanan
        right_pos_shoulder = right_shoulder(pitch3)
        right_pos_upper_arm = arm(pitch1, right_upper_arm_angle, right_upper_arm_position)
        right_pos_lower_arm = arm(pitch2, right_lower_arm_angle, right_lower_arm_position)
        
        #kiri
        left_pos_shoulder = left_shoulder(pitch6)
        left_pos_upper_arm = arm(pitch4, left_upper_arm_angle, left_upper_arm_position)
        left_pos_lower_arm = arm(pitch2, left_lower_arm_angle, left_lower_arm_position)
        
        # Mengatur Position
        set_goal_position(DXL_ID_4, right_pos_shoulder) # kanan
        set_goal_position(DXL_ID_3, right_pos_upper_arm)
        set_goal_position(DXL_ID_2, right_pos_lower_arm)

        set_goal_position(DXL_ID_5, left_pos_shoulder) # kiri
        set_goal_position(DXL_ID_6, left_pos_upper_arm)
        set_goal_position(DXL_ID_7, left_pos_lower_arm)

        # print(f"Servo 1: {pos_shoulder}, Servo 2: {pos_upper_arm}")

except KeyboardInterrupt:
    print("\nTerminal ditutup oleh User.")

# Menonaktifkan torque
packetHandler.write1ByteTxRx(portHandler, DXL_ID_4, ADDR_MX_TORQUE_ENABLE, TORQUE_DISABLE) # kanan
packetHandler.write1ByteTxRx(portHandler, DXL_ID_3, ADDR_MX_TORQUE_ENABLE, TORQUE_DISABLE)
packetHandler.write1ByteTxRx(portHandler, DXL_ID_2, ADDR_MX_TORQUE_ENABLE, TORQUE_DISABLE)

packetHandler.write1ByteTxRx(portHandler, DXL_ID_5, ADDR_MX_TORQUE_ENABLE, TORQUE_DISABLE) # kiri
packetHandler.write1ByteTxRx(portHandler, DXL_ID_6, ADDR_MX_TORQUE_ENABLE, TORQUE_DISABLE)
packetHandler.write1ByteTxRx(portHandler, DXL_ID_7, ADDR_MX_TORQUE_ENABLE, TORQUE_DISABLE)

portHandler.closePort()
sock.close()