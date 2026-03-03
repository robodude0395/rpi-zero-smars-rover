# Requirements Document

## Introduction

This document specifies requirements for adding keyboard control functionality to the tracked tank rover system. The feature enables a human operator to control the rover's movement in real-time using keyboard input on the Raspberry Pi master controller. The keyboard control will translate key presses into motor commands sent via the existing SPI communication protocol to the Arduino slave controller.

## Glossary

- **Rover_Controller**: The Python class on the Raspberry Pi that manages SPI communication with the Arduino
- **Keyboard_Handler**: The component responsible for capturing and processing keyboard input
- **Motor_Command**: A 3-byte SPI packet containing command ID and motor speed values
- **Forward_Movement**: Both motors running at equal positive speed
- **Backward_Movement**: Both motors running at equal negative speed
- **Left_Turn**: Left motor stopped or reversed while right motor runs forward
- **Right_Turn**: Right motor stopped or reversed while left motor runs forward
- **Stop_State**: Both motors set to zero speed
- **Control_Loop**: The main execution loop that processes keyboard input and sends commands
- **Base_Speed**: The default motor speed value used for movement commands (0-127 range)

## Requirements

### Requirement 1: Keyboard Input Capture

**User Story:** As an operator, I want to press keys to control the rover, so that I can navigate it in real-time.

#### Acceptance Criteria

1. WHEN the Control_Loop is running, THE Keyboard_Handler SHALL capture key presses without blocking execution
2. THE Keyboard_Handler SHALL detect key release events to stop movement
3. WHEN multiple keys are pressed simultaneously, THE Keyboard_Handler SHALL process the most recent key press
4. THE Keyboard_Handler SHALL operate without requiring Enter key confirmation

### Requirement 2: Forward and Backward Movement

**User Story:** As an operator, I want to move the rover forward and backward, so that I can navigate along a straight path.

#### Acceptance Criteria

1. WHEN the 'w' key is pressed, THE Rover_Controller SHALL send a Motor_Command for Forward_Movement at Base_Speed
2. WHEN the 's' key is pressed, THE Rover_Controller SHALL send a Motor_Command for Backward_Movement at Base_Speed
3. WHEN the 'w' or 's' key is released, THE Rover_Controller SHALL send a Motor_Command for Stop_State
4. WHILE the 'w' key is held, THE Rover_Controller SHALL continuously send Forward_Movement commands at the configured update rate

### Requirement 3: Turning Control

**User Story:** As an operator, I want to turn the rover left and right, so that I can change direction.

#### Acceptance Criteria

1. WHEN the 'a' key is pressed, THE Rover_Controller SHALL send a Motor_Command for Left_Turn
2. WHEN the 'd' key is pressed, THE Rover_Controller SHALL send a Motor_Command for Right_Turn
3. WHEN the 'a' or 'd' key is released, THE Rover_Controller SHALL send a Motor_Command for Stop_State
4. WHERE tank-style turning is configured, THE Left_Turn SHALL reverse the left motor while running the right motor forward at Base_Speed

### Requirement 4: Emergency Stop

**User Story:** As an operator, I want an immediate stop command, so that I can halt the rover in emergency situations.

#### Acceptance Criteria

1. WHEN the spacebar key is pressed, THE Rover_Controller SHALL immediately send a Motor_Command for Stop_State
2. THE Rover_Controller SHALL send the Stop_State command within 50 milliseconds of spacebar detection
3. WHEN the spacebar is pressed, THE Rover_Controller SHALL override any other active movement command

### Requirement 5: Graceful Exit

**User Story:** As an operator, I want to exit the control program safely, so that the rover stops and resources are released properly.

#### Acceptance Criteria

1. WHEN the 'q' key is pressed, THE Control_Loop SHALL terminate execution
2. WHEN the Control_Loop terminates, THE Rover_Controller SHALL send a Motor_Command for Stop_State
3. WHEN the Control_Loop terminates, THE Rover_Controller SHALL close the SPI connection
4. IF a keyboard interrupt signal is received, THEN THE Control_Loop SHALL execute the same termination sequence as the 'q' key

### Requirement 6: Speed Configuration

**User Story:** As an operator, I want to adjust movement speed, so that I can control the rover at different speeds for different situations.

#### Acceptance Criteria

1. THE Base_Speed SHALL be configurable as a parameter to the Keyboard_Handler
2. THE Base_Speed SHALL accept values in the range 0 to 127
3. WHEN Base_Speed is set to 0, THE Rover_Controller SHALL send Motor_Commands with speed value 128 (stopped)
4. WHEN Base_Speed is set to 127, THE Rover_Controller SHALL send Motor_Commands with speed value 255 (maximum forward)

### Requirement 7: Visual Feedback

**User Story:** As an operator, I want to see the current command on screen, so that I can confirm my input is being processed.

#### Acceptance Criteria

1. WHEN a movement key is pressed, THE Control_Loop SHALL display the current movement direction on the terminal
2. WHEN the rover enters Stop_State, THE Control_Loop SHALL display "STOPPED" on the terminal
3. THE Control_Loop SHALL display help text showing available key commands when starting
4. THE Control_Loop SHALL update the display without clearing previous output more than once per second

### Requirement 8: Command Rate Limiting

**User Story:** As a system integrator, I want commands sent at a consistent rate, so that the Arduino slave controller receives predictable timing.

#### Acceptance Criteria

1. THE Control_Loop SHALL send Motor_Commands at a configurable update rate
2. THE Control_Loop SHALL default to 10 commands per second when no rate is specified
3. WHILE no key is pressed, THE Control_Loop SHALL continue sending the last command state at the configured rate
4. THE Control_Loop SHALL maintain timing accuracy within 10 milliseconds of the target interval
