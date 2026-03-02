from smbus2 import SMBus, i2c_msg

I2C_ADDR = 0x08
BUS = 1

def send_motor(left, right):
    left_byte = left + 128
    right_byte = right + 128

    data = bytes([0x01, left_byte, right_byte])

    with SMBus(BUS) as bus:
        msg = i2c_msg.write(I2C_ADDR, data)
        bus.i2c_rdwr(msg)


# Example usage
send_motor(80, 80)    # Forward
send_motor(-80, 80)   # Turn
send_motor(0, 0)      # Stop