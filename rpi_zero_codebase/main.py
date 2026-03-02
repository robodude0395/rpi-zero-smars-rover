from smbus2 import SMBus
import time

I2C_ADDR = 0x08
BUS = 1

def send_motor(bus, left, right):
    left_byte = left + 128
    right_byte = right + 128

    try:
        bus.write_i2c_block_data(I2C_ADDR, 0, [0x01, left_byte, right_byte])
    except OSError:
        print("I2C write failed (Arduino busy?)")

with SMBus(BUS) as bus:

    while True:
        send_motor(bus, 80, 80)   # Forward
        time.sleep(0.1)           # 10 Hz update rate