# Technology Stack

## Arduino Component

**Platform**: PlatformIO with Arduino framework
**Board**: Arduino Pro Mini (ATmega328P @ 16MHz)
**Build System**: PlatformIO

### Libraries
- `Adafruit SSD1306` - OLED display driver
- `Adafruit GFX Library` - Graphics primitives
- `Wire` - I2C communication (built-in)

### Common Commands

```bash
# Build the project
pio run

# Upload to Arduino
pio run --target upload

# Clean build files
pio run --target clean

# Monitor serial output
pio device monitor
```

### Configuration
- Upload speed: 57600 baud
- Environment: `pro16` (defined in platformio.ini)

## Raspberry Pi Component

**Language**: Python 3
**Location**: `rpi_zero_codebase/`

### Dependencies
- `smbus2` - I2C communication library
- `readchar` - Keyboard input handling

### Setup & Run

```bash
# Install dependencies
pip install -r rpi_zero_codebase/requirements.txt

# Run the control script
python rpi_zero_codebase/main.py
```

## Hardware Communication

**Protocol**: I2C
**Arduino Address**: 0x08
**Data Format**: 2-byte commands (left motor, right motor) as signed values (-128 to 127)
