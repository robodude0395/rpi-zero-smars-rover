from smbus2 import SMBus, i2c_msg
import time

I2C_ADDR = 0x08
BUS_NUM = 1   # 1 for Pi Zero

def send_message(message):
    # Limit to 31 chars (Arduino buffer safety)
    message = message[:31]

    # Convert to bytes
    data = message.encode('utf-8')

    # Create I2C write message
    msg = i2c_msg.write(I2C_ADDR, data)

    with SMBus(BUS_NUM) as bus:
        bus.i2c_rdwr(msg)

if __name__ == "__main__":
    while True:
        text = input("Enter message: ")
        send_message(text)
        time.sleep(0.1)