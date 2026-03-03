import spidev
import time
import readchar

SPI_BUS = 0          # SPI bus 0
SPI_DEVICE = 0       # CE0
SPI_SPEED = 500000   # 500 kHz
UPDATE_RATE = 0.1    # 10 Hz (100ms)
BASE_SPEED = 127      # Default motor speed (0-BASE_SPEED range)

# Key mapping: key -> (left_motor_speed, right_motor_speed)
KEY_COMMANDS = {
    'w': (BASE_SPEED, BASE_SPEED),      # Forward
    's': (-BASE_SPEED, -BASE_SPEED),    # Backward
    'a': (-BASE_SPEED, BASE_SPEED),     # Left turn (tank style)
    'd': (BASE_SPEED, -BASE_SPEED),     # Right turn (tank style)
    ' ': (0, 0),        # Stop (spacebar)
    'q': None           # Quit
}

class RoverController:
    def __init__(self, bus=0, device=0, speed=500000):
        self.spi = spidev.SpiDev()
        self.spi.open(bus, device)
        self.spi.max_speed_hz = speed
        self.spi.mode = 0  # SPI mode 0 (CPOL=0, CPHA=0)
        self.command_id = 0

    def send_motor(self, left, right):
        # Clamp speeds to -BASE_SPEED to BASE_SPEED
        left = max(-BASE_SPEED, min(BASE_SPEED, left))
        right = max(-BASE_SPEED, min(BASE_SPEED, right))

        # Convert to unsigned byte (128 offset encoding)
        left_byte = left + 128
        right_byte = right + 128

        # Increment command ID
        self.command_id = (self.command_id + 1) % 256

        # Send 3-byte packet: [command_id, left_speed, right_speed]
        try:
            self.spi.xfer2([self.command_id, left_byte, right_byte])
        except Exception as e:
            print(f"SPI write failed: {e}")

    def stop(self):
        self.send_motor(0, 0)

    def close(self):
        self.spi.close()


# ---------------- MAIN LOOP ---------------- #

if __name__ == "__main__":

    rover = RoverController(SPI_BUS, SPI_DEVICE, SPI_SPEED)

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