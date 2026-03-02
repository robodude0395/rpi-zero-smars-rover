# Project Structure

## Root Layout

```
rpi-zero-smars-rover/
├── src/                    # Arduino source code
├── include/                # Arduino header files (currently unused)
├── lib/                    # Arduino custom libraries (currently unused)
├── test/                   # Test files (currently unused)
├── rpi_zero_codebase/      # Raspberry Pi Python code
├── .pio/                   # PlatformIO build artifacts and dependencies
├── .vscode/                # VS Code configuration
└── platformio.ini          # PlatformIO project configuration
```

## Arduino Component (`src/`)

**Main file**: `src/main.cpp`

Code is organized into logical sections with clear comment headers:
- Motor control functions
- I2C event handlers (receive/request)
- Battery monitoring
- Display rendering helpers
- Setup and main loop

### Pin Assignments
- **Left Motor**: IN1=4, IN2=5, PWM=6
- **Right Motor**: IN1=7, IN2=8, PWM=9
- **Battery**: Analog pin A0
- **I2C**: SDA/SCL (default Wire pins)
- **OLED**: I2C address 0x3C

## Raspberry Pi Component (`rpi_zero_codebase/`)

**Main file**: `rpi_zero_codebase/main.py` - Single-file keyboard control script
**Dependencies**: `rpi_zero_codebase/requirements.txt`

Simple structure with inline control logic and I2C communication.

## Build Artifacts

`.pio/` contains:
- `build/` - Compiled binaries and object files
- `libdeps/` - Downloaded library dependencies (Adafruit libraries)

This directory is managed by PlatformIO and should not be manually edited.

## Conventions

- Arduino code uses clear section headers with `/* ===== */` style
- Pin definitions use `#define` constants at the top
- Motor values are constrained to -255 to +255 range
- I2C communication uses unsigned byte transmission with signed interpretation
- Display updates and battery reads use non-blocking timers (millis-based)
