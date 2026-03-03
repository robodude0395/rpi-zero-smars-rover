from smbus2 import SMBus
import time
import readchar

I2C_ADDR = 0x08
BUS_NUM = 1          # Pi Zero uses bus 1
UPDATE_RATE = 0.1    # 10 Hz (100ms)
BASE_SPEED = 80      # Default motor speed (0-127 range)

# Key mapping: key -> (left_motor_speed, right_motor_speed)
KEY_COMMANDS = {
    'w': (80, 80),      # Forward
    's': (-80, -80),    # Backward
    'a': (-80, 80),     # Left turn (tank style)
    'd': (80, -80),     # Right turn (tank style)
    ' ': (0, 0),        # Stop (spacebar)
    'q': None           # Quit
}

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

    # Display help text
    print("Keyboard Controls:")
    print("  W - Forward")
    print("  S - Backward")
    print("  A - Turn Left")
    print("  D - Turn Right")
    print("  SPACE - Stop")
    print("  Q - Quit")
    print()

    last_command = (0, 0)

    try:
        while True:
            # Non-blocking key read
            try:
                key = readchar.readkey()
            except:
                key = None

            # Process key
            if key == 'q':
                break
            elif key in KEY_COMMANDS:
                command = KEY_COMMANDS[key]
                if command:
                    rover.send_motor(*command)
                    last_command = command
            else:
                # No key or unknown key - send last command
                rover.send_motor(*last_command)

            # Simple feedback
            if last_command == (0, 0):
                print("STOPPED", end='\r')
            else:
                print(f"L:{last_command[0]:4d} R:{last_command[1]:4d}", end='\r')

            time.sleep(UPDATE_RATE)

    except KeyboardInterrupt:
        pass
    finally:
        print("\nStopping rover...")
        rover.stop()
        rover.close()