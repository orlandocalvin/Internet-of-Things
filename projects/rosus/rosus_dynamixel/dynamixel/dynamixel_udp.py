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
DXL_ID = 1

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

# Mengaktifkan torque
dxl_comm_result, dxl_error = packetHandler.write1ByteTxRx(portHandler, DXL_ID, ADDR_MX_TORQUE_ENABLE, TORQUE_ENABLE)

def deg_to_position(pitch):
    position = int((pitch + 90) / 180 * 1023)
    return max(0, min(position, 1023))

try:
    while True:
        data, addr = sock.recvfrom(1024)
        json_data = json.loads(data.decode())

        dxl_goal_position = deg_to_position(json_data['pitch'])

        # Memperbarui posisi servo
        dxl_comm_result, dxl_error = packetHandler.write2ByteTxRx(portHandler, DXL_ID, ADDR_MX_GOAL_POSITION, dxl_goal_position)
        print(f"Goal Position: {dxl_goal_position}")

except KeyboardInterrupt:
    # Menonaktifkan torque
    dxl_comm_result, dxl_error = packetHandler.write1ByteTxRx(portHandler, DXL_ID, ADDR_MX_TORQUE_ENABLE, TORQUE_DISABLE)
    portHandler.closePort()
    sock.close()