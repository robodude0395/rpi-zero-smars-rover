import smbus
import time

I2C_ADDR = 0x08
BUS = 1  # 1 for modern Raspberry Pi

bus = smbus.SMBus(BUS)

def send_message(message):
    # Convert string to list of ASCII values
    data = [ord(c) for c in message]

    # Send in one block (max 32 bytes recommended)
    bus.write_i2c_block_data(I2C_ADDR, 0x00, data)

if __name__ == "__main__":
    while True:
        msg = input("Enter message: ")
        send_message(msg[:31])  # limit to 31 chars
        time.sleep(0.1)