---
inclusion: always
---

# Product Overview

SPI-controlled tracked tank rover with dual-controller architecture:

- **Master**: Raspberry Pi Zero (Python) sends motor commands via SPI
- **Slave**: Arduino Pro Mini (ATmega328P) controls motors and display
- **Display**: SSD1306 128x64 I2C OLED showing motor state and battery level
- **Motors**: Two DC motors driven by L293D dual H-bridge

The system uses non-blocking timing patterns and implements safety timeouts to stop motors if communication is lost.
