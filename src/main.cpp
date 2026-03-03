#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>

/* ---------- OLED Display ---------- */
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define OLED_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

/* ---------- Motor Driver Pins (L293D) ---------- */
#define EN1 9   // Motor 1 Enable (PWM)
#define IN1 6   // Motor 1 Direction A
#define IN2 7   // Motor 1 Direction B

#define EN2 3   // Motor 2 Enable (PWM)
#define IN3 8   // Motor 2 Direction A
#define IN4 4   // Motor 2 Direction B

/* ---------- SPI Pins ---------- */
#define SPI_CS 2    // Custom chip select
// Pin 10 (hardware SS) must remain INPUT for SPI slave mode
// MOSI = 11, MISO = 12, SCK = 13 (hardware SPI)

/* ---------- Communication Protocol ---------- */
#define CMD_BUFFER_SIZE 3
volatile byte spiBuffer[CMD_BUFFER_SIZE];
volatile byte spiIndex = 0;
volatile bool newCommand = false;

/* ---------- Safety Timeout ---------- */
#define TIMEOUT_MS 1000
unsigned long lastCommandTime = 0;

/* ---------- Display Update ---------- */
#define DISPLAY_UPDATE_MS 100
unsigned long lastDisplayUpdate = 0;

/* ---------- Current Motor State ---------- */
int currentLeftSpeed = 0;
int currentRightSpeed = 0;
byte lastCommandId = 0;

/* ---------- Motor Control Functions ---------- */
void setMotor(int en, int in1, int in2, int speed) {
  speed = constrain(speed, -255, 255);

  if (speed > 0) {
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
    analogWrite(en, speed);
  }
  else if (speed < 0) {
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
    analogWrite(en, -speed);
  }
  else {
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
    analogWrite(en, 0);
  }
}

void setLeftMotor(int speed) {
  currentLeftSpeed = speed;
  setMotor(EN1, IN1, IN2, speed);
}

void setRightMotor(int speed) {
  currentRightSpeed = speed;
  setMotor(EN2, IN3, IN4, speed);
}

void stopMotors() {
  setLeftMotor(0);
  setRightMotor(0);
}

/* ---------- SPI Interrupt Handler ---------- */
ISR(SPI_STC_vect) {
  byte received = SPDR;

  if (spiIndex < CMD_BUFFER_SIZE) {
    spiBuffer[spiIndex++] = received;

    if (spiIndex >= CMD_BUFFER_SIZE) {
      newCommand = true;
      spiIndex = 0;
    }
  }

  SPDR = 0; // Send dummy byte back
}

/* ---------- Process SPI Command ---------- */
void processCommand() {
  if (!newCommand) return;

  // Decode 3-byte command packet
  byte cmdId = spiBuffer[0];
  byte leftByte = spiBuffer[1];
  byte rightByte = spiBuffer[2];

  // Convert from offset encoding (128 = stop)
  int leftSpeed = (int)leftByte - 128;
  int rightSpeed = (int)rightByte - 128;

  // Scale to -255 to 255 range
  leftSpeed = map(leftSpeed, -128, 127, -255, 255);
  rightSpeed = map(rightSpeed, -128, 127, -255, 255);

  // Apply motor commands
  setLeftMotor(leftSpeed);
  setRightMotor(rightSpeed);

  lastCommandId = cmdId;
  lastCommandTime = millis();
  newCommand = false;
}

/* ---------- Display Update ---------- */
void updateDisplay() {
  unsigned long now = millis();
  if (now - lastDisplayUpdate < DISPLAY_UPDATE_MS) return;

  lastDisplayUpdate = now;

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  // Title
  display.setCursor(0, 0);
  display.println("ROVER STATUS");
  display.drawLine(0, 10, SCREEN_WIDTH, 10, SSD1306_WHITE);

  // Motor speeds
  display.setCursor(0, 15);
  display.print("L: ");
  display.print(currentLeftSpeed);
  display.print("  R: ");
  display.println(currentRightSpeed);

  // Command ID
  display.setCursor(0, 28);
  display.print("CMD: ");
  display.println(lastCommandId);

  // Connection status
  display.setCursor(0, 41);
  if (millis() - lastCommandTime < TIMEOUT_MS) {
    display.println("Status: CONNECTED");
  } else {
    display.println("Status: TIMEOUT");
  }

  // Battery placeholder
  display.setCursor(0, 54);
  display.print("Battery: OK");

  display.display();
}

/* ---------- Safety Check ---------- */
void checkTimeout() {
  if (millis() - lastCommandTime > TIMEOUT_MS) {
    if (currentLeftSpeed != 0 || currentRightSpeed != 0) {
      stopMotors();
    }
  }
}

/* ---------- Setup ---------- */
void setup() {
  // Motor pins
  pinMode(EN1, OUTPUT);
  pinMode(EN2, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  // Safe startup - motors off
  stopMotors();

  // SPI slave setup
  pinMode(MISO, OUTPUT);
  pinMode(SPI_CS, INPUT);
  pinMode(10, INPUT); // Hardware SS must be INPUT for slave mode

  SPCR |= _BV(SPE);   // Enable SPI in slave mode
  SPCR |= _BV(SPIE);  // Enable SPI interrupt

  SPI.attachInterrupt();

  // I2C and OLED
  Wire.begin();
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
    // OLED init failed - continue anyway
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("ROVER READY");
  display.println("Waiting for SPI...");
  display.display();

  lastCommandTime = millis();
}

/* ---------- Main Loop ---------- */
void loop() {
  processCommand();
  checkTimeout();
  updateDisplay();
}