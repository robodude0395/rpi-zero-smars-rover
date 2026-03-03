# Design Document: Basic Keyboard Control

## Overview

This design adds keyboard control to the existing rover system by extending the `rpi_zero_codebase/main.py` file. The implementation uses the `readchar` library to capture non-blocking keyboard input and maps key presses to motor commands sent via the existing I2C communication protocol.

The design prioritizes simplicity: all functionality is added to the existing main.py file with minimal code changes. No new files, no test infrastructure, just straightforward keyboard-to-motor mapping.

## Architecture

### Single-File Extension

The entire feature is implemented by modifying `rpi_zero_codebase/main.py`:

1. Add `readchar` import for non-blocking keyboard input
2. Create a key-to-command mapping dictionary
3. Replace the example loop with a keyboard control loop
4. Add simple terminal output for operator feedback

### Control Flow

```
Start Program
    ↓
Display Help Text
    ↓
Enter Control Loop (10 Hz)
    ↓
Check for Key Press (non-blocking)
    ↓
Map Key → Motor Command
    ↓
Send Command via I2C
    ↓
Display Current State
    ↓
Sleep to Maintain Rate
    ↓
Repeat or Exit on 'q'
```

## Components and Interfaces

### Modified RoverController Class

No changes needed to the existing `RoverController` class. It already provides:
- `send_motor(left, right)`: Send motor speeds (-127 to 127)
- `stop()`: Stop both motors
- `close()`: Clean up I2C connection

### Keyboard Input Handling

Use `readchar.readkey()` with try/except to capture keys without blocking:

```python
try:
    key = readchar.readkey()
except:
    key = None  # No key pressed
```

### Key Mapping

Simple dictionary mapping keys to motor speed tuples:

```python
KEY_COMMANDS = {
    'w': (80, 80),      # Forward
    's': (-80, -80),    # Backward
    'a': (-80, 80),     # Left turn (tank style)
    'd': (80, -80),     # Right turn (tank style)
    ' ': (0, 0),        # Stop (spacebar)
    'q': None           # Quit
}
```

Base speed of 80 provides good control without being too aggressive.

## Data Models

### Command State

Track the last command sent to maintain consistent output:

```python
last_command = (0, 0)  # (left_speed, right_speed)
```

### Key Press State

No state tracking needed - each loop iteration processes the current key press independently.

## Error Handling

### I2C Communication Errors

Already handled by existing `RoverController.send_motor()` method - prints error message and continues.

### Keyboard Input Errors

Catch exceptions from `readchar.readkey()` and treat as "no key pressed".

### Exit Handling

Ensure motors stop on both normal exit ('q' key) and interrupt (Ctrl+C):

```python
try:
    # Main loop
except KeyboardInterrupt:
    pass
finally:
    rover.stop()
    rover.close()
```

## Implementation Details

### Main Loop Structure

```python
UPDATE_RATE = 0.1  # 10 Hz
BASE_SPEED = 80

# Display help
print("Keyboard Controls:")
print("  W - Forward")
print("  S - Backward")
print("  A - Turn Left")
print("  D - Turn Right")
print("  SPACE - Stop")
print("  Q - Quit")

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
```

### Terminal Output

Use `end='\r'` to overwrite the same line, keeping output clean. Show motor speeds in a simple format that updates in place.

### Timing

The `time.sleep(UPDATE_RATE)` maintains 10 Hz command rate. This is simple and sufficient - no need for precise timing control.

## Dependencies

Add to `rpi_zero_codebase/requirements.txt`:

```
smbus2
readchar
```

The `readchar` library provides cross-platform non-blocking keyboard input without requiring terminal mode changes.

## Summary

This design adds keyboard control with approximately 40 lines of code added to the existing main.py file. No new files, no complex state management, no test infrastructure. The operator gets immediate tactile control of the rover with visual feedback on the terminal.
