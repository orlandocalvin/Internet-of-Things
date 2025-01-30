import dynamixel_sdk as dxl

DEVICENAME = 'COM4'
BAUDRATE = 57600
PROTOCOL_VERSION = 1.0
DXL_ID = 8

ADDR_MX_TORQUE_ENABLE = 24
# ADDR_MX_GOAL_POSITION = 30
ADDR_MOVING_SPEED = 32
CCW_ANGLE_LIMIT = 8

torque_enable = 1
torque_disable = 0
wheel_mode = 0
# joint_mode = 1023

def clockwise(value):
    return value + 1024

dxl_moving_speed_off = 0
dxl_moving_speed = clockwise(200)

portHandler = dxl.PortHandler(DEVICENAME)
packetHandler = dxl.PacketHandler(PROTOCOL_VERSION)

portHandler.openPort()
portHandler.setBaudRate(BAUDRATE)

packetHandler.write2ByteTxRx(portHandler, DXL_ID, CCW_ANGLE_LIMIT, wheel_mode)
packetHandler.write1ByteTxRx(portHandler, DXL_ID, ADDR_MX_TORQUE_ENABLE, torque_enable)
packetHandler.write2ByteTxRx(portHandler, DXL_ID, ADDR_MOVING_SPEED, dxl_moving_speed)

try:
    print("Servo Moving!\nPress Ctrl+C to stop the servo..")
    while True:
        pass

except KeyboardInterrupt:
    packetHandler.write2ByteTxRx(portHandler, DXL_ID, ADDR_MOVING_SPEED, dxl_moving_speed_off)
    packetHandler.write1ByteTxRx(portHandler, DXL_ID, ADDR_MX_TORQUE_ENABLE, torque_disable)
    portHandler.closePort()
    
    print("\nServo Stopped!")