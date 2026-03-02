from smbus2 import SMBus
import readchar

ADDR = 0x08
bus = SMBus(1)

speed = 80

left = 0
right = 0

def send():
    bus.write_i2c_block_data(
        ADDR, 0,
        [left & 0xFF, right & 0xFF]
    )

print("""
W/S = forward/back
A/D = turn
SPACE = stop
Q = quit
""")

while True:
    key = readchar.readkey()

    if key == 'w':
        left = speed
        right = speed

    elif key == 's':
        left = -speed
        right = -speed

    elif key == 'a':
        left = -speed
        right = speed

    elif key == 'd':
        left = speed
        right = -speed

    elif key == ' ':
        left = 0
        right = 0

    elif key == 'q':
        break

    send()
    print("L:", left, "R:", right)

bus.write_i2c_block_data(ADDR,0,[0,0])