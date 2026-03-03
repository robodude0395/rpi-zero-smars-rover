---
inclusion: always
---

# Project Structure

```
.
├── src/                    # Arduino source code
│   └── main.cpp           # Main Arduino firmware (motor control, OLED, SPI slave)
├── rpi_zero_codebase/     # Raspberry Pi master controller
│   ├── main.py            # Python SPI master implementation
│   └── requirements.txt   # Python dependencies
├── include/               # Arduino header files (if needed)
├── lib/                   # Custom Arduino libraries (if needed)
├── test/                  # Test files
├── .pio/                  # PlatformIO build artifacts and dependencies
│   ├── build/            # Compiled binaries
│   └── libdeps/          # Downloaded libraries (Adafruit SSD1306, GFX)
└── platformio.ini         # PlatformIO configuration
```

## Code Organization

### Arduino (src/main.cpp)
- Motor control functions: `setMotor()`, `setLeftMotor()`, `setRightMotor()`, `stopMotors()`
- Display functions: `showCommand()`
- Pin definitions for L293D motor driver
- Non-blocking loop using `millis()` for timing

### Raspberry Pi (rpi_zero_codebase/main.py)
- `RoverController` class encapsulates SPI communication
- Motor command encoding/decoding
- Main control loop with configurable update rate

## Hardware Pin Mapping

### Motor Driver (L293D)
- Motor 1: EN1=9, IN1=6, IN2=7
- Motor 2: EN2=3, IN3=8, IN4=4

### SPI (RPi → Arduino)
- MOSI → Pin 11
- MISO → Pin 12
- SCLK → Pin 13
- CE0 → Pin 2 (custom CS)
- **Important**: Pin 10 (hardware SS) must remain INPUT

### I2C OLED
- Address: 0x3C
- Connected via Wire library
