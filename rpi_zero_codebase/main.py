from smbus2 import SMBus
import time

I2C_ADDR = 0x08
BUS_NUM = 1          # Pi Zero uses bus 1
UPDATE_RATE = 0.1    # 10 Hz (100ms)

class RoverController:
    def __init__(self, bus_num=1, addr=0x08):
        self.bus = SMBus(bus_num)
        self.addr = addr

    def send_motor(self, left, right):
        # Clamp speeds
        left = max(-127, min(127, left))
        right = max(-127, min(127, right))

        # Convert to unsigned byte
        left_byte = left + 128
        right_byte = right + 128

        try:
            self.bus.write_i2c_block_data(
                self.addr,
                0x00,                    # command register (ignored by Arduino)
                [0x01, left_byte, right_byte]
            )
        except OSError:
            print("I2C write failed (Arduino busy or disconnected)")

    def stop(self):
        self.send_motor(0, 0)

    def close(self):
        self.bus.close()


# ---------------- MAIN LOOP ---------------- #

if __name__ == "__main__":

    rover = RoverController(BUS_NUM, I2C_ADDR)

    try:
        while True:
            # Example: forward
            rover.send_motor(80, 80)
            time.sleep(UPDATE_RATE)

    except KeyboardInterrupt:
        print("Stopping rover...")
        rover.stop()
        rover.close()