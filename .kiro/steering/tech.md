---
inclusion: always
---

# Technology Stack

## Arduino (Slave Controller)

- **Platform**: ATmega328P (Arduino Pro Mini 16MHz)
- **Build System**: PlatformIO
- **Framework**: Arduino
- **Libraries**:
  - Adafruit SSD1306 (OLED display)
  - Adafruit GFX Library (graphics primitives)
  - Wire (I2C communication)

### Common Commands

```bash
# Build Arduino firmware
pio run

# Upload to Arduino Pro Mini
pio run --target upload

# Clean build artifacts
pio run --target clean

# Monitor serial output
pio device monitor
```

## Raspberry Pi (Master Controller)

- **Platform**: Raspberry Pi Zero
- **Language**: Python 3
- **Dependencies**:
  - smbus2 (I2C communication)
  - readchar (keyboard input)

### Common Commands

```bash
# Install Python dependencies
pip install -r rpi_zero_codebase/requirements.txt

# Run master controller
python rpi_zero_codebase/main.py
```

## Communication Protocol

- **Primary**: SPI (Pi master → Arduino slave)
- **Display**: I2C (Arduino → OLED at 0x3C)
- **Command Structure**: 3-byte packets
  - Byte 0: Command ID
  - Byte 1: Left motor speed (0-255, offset by 128)
  - Byte 2: Right motor speed (0-255, offset by 128)
- **Speed Encoding**: 128 = stop, >128 = forward, <128 = backward
