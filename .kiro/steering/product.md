# Product Overview

This is a SMARS rover project controlled by a Raspberry Pi Zero. The system consists of two components:

1. **Arduino Pro Mini (ATmega328P)** - Motor controller and display driver that receives commands via I2C
2. **Raspberry Pi Zero** - Master controller that sends motor commands and reads battery status

The Arduino manages:
- Dual motor control (left/right) with PWM speed control
- OLED display (128x64 SSD1306) showing battery level and motor status
- Battery voltage monitoring via analog input
- I2C slave communication (address 0x08)

The Raspberry Pi provides:
- Keyboard-based control interface (WASD + Space/Q)
- I2C master communication to send motor commands

This is a typical embedded robotics project for educational or hobby rover applications.
