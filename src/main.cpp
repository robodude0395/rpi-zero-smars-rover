unsigned long lastCommandTime = 0;

void loop() {

  if (newCommand) {
    if (cmdBuffer[0] == 0x01) {
      leftSpeed = (int)cmdBuffer[1] - 128;
      rightSpeed = (int)cmdBuffer[2] - 128;
      lastCommandTime = millis();
    }
    newCommand = false;
  }

  // Safety timeout (500ms)
  if (millis() - lastCommandTime > 500) {
    leftSpeed = 0;
    rightSpeed = 0;
  }

  updateMotors();
}