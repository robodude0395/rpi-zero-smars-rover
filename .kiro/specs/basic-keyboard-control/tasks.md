# Implementation Plan: Basic Keyboard Control

## Overview

Add keyboard control functionality to the existing rover system by modifying `rpi_zero_codebase/main.py`. The implementation adds approximately 40 lines of code to enable WASD keyboard control with visual feedback. Uses the `readchar` library for non-blocking keyboard input.

## Tasks

- [x] 1. Update Python dependencies
  - Add `readchar` to `rpi_zero_codebase/requirements.txt`
  - _Requirements: 1.1, 1.4_

- [x] 2. Implement keyboard control in main.py
  - [x] 2.1 Add readchar import and key mapping dictionary
    - Import `readchar` library at top of file
    - Create `KEY_COMMANDS` dictionary mapping keys to motor speed tuples
    - Define `UPDATE_RATE` and `BASE_SPEED` constants
    - _Requirements: 1.1, 1.3, 6.1, 6.2_

  - [x] 2.2 Add help text display function
    - Display keyboard control instructions when program starts
    - Show available commands: W/S/A/D/SPACE/Q
    - _Requirements: 7.3_

  - [x] 2.3 Implement main keyboard control loop
    - Replace existing example loop with keyboard control loop
    - Add non-blocking key reading with try/except
    - Process key presses and map to motor commands
    - Handle 'q' key for graceful exit
    - Maintain last command state for continuous sending
    - _Requirements: 1.1, 1.2, 2.1, 2.3, 3.1, 3.2, 4.1, 4.3, 5.1, 8.1, 8.3_

  - [x] 2.4 Add terminal feedback display
    - Show current motor speeds using carriage return for in-place updates
    - Display "STOPPED" when motors are at zero speed
    - Format output as "L:XXX R:XXX" for motor speeds
    - _Requirements: 7.1, 7.2, 7.4_

  - [x] 2.5 Implement exit handling
    - Add try/except/finally block for KeyboardInterrupt
    - Ensure motors stop on both 'q' key and Ctrl+C
    - Close I2C connection properly on exit
    - _Requirements: 5.1, 5.2, 5.3, 5.4_

- [x] 3. Verify implementation
  - Run the modified main.py and test keyboard controls
  - Ensure all keys (W/S/A/D/SPACE/Q) work as expected
  - Confirm graceful exit with motor stop
  - _Requirements: All_

## Notes

- No test tasks included per user request for minimal implementation
- All changes are in existing `rpi_zero_codebase/main.py` file
- Base speed set to 80 (out of 127 max) for safe initial testing
- Control loop runs at 10 Hz (UPDATE_RATE = 0.1 seconds)
- Tank-style turning: left turn reverses left motor, right turn reverses right motor
