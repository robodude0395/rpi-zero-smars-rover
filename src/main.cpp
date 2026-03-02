#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

/* ---------- OLED ---------- */
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

/* ---------- I2C ---------- */
#define I2C_ADDR 0x08

/* ---------- Motor Pins ---------- */
#define EN1 9
#define IN1 6
#define IN2 7

#define EN2 10
#define IN3 8
#define IN4 4

/* ---------- State ---------- */
volatile uint8_t cmdBuffer[3];
volatile bool newCommand = false;

int leftSpeed = 0;
int rightSpeed = 0;

unsigned long lastBatteryRead = 0;
unsigned long lastDisplayUpdate = 0;

/* ---------- I2C Receive ---------- */
void receiveEvent(int howMany) {
  if (howMany >= 3) {
    cmdBuffer[0] = Wire.read();
    cmdBuffer[1] = Wire.read();
    cmdBuffer[2] = Wire.read();
    newCommand = true;
  }
}

/* ---------- Motor Control ---------- */
void setMotor(int en, int in1, int in2, int speed) {
  if (speed > 0) {
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
    analogWrite(en, speed);
  }
  else if (speed < 0) {
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
    analogWrite(en, -speed);
  }
  else {
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
    analogWrite(en, 0);
  }
}

void updateMotors() {
  setMotor(EN1, IN1, IN2, leftSpeed);
  setMotor(EN2, IN3, IN4, rightSpeed);
}

/* ---------- Battery ---------- */
int batteryPercent = 0;

void readBattery() {
  int raw = analogRead(A0);

  // Assuming 2S Li-ion (adjust!)
  float voltage = raw * (5.0 / 1023.0);

  batteryPercent = map(voltage * 100, 640, 840, 0, 100);
  batteryPercent = constrain(batteryPercent, 0, 100);
}

/* ---------- Display ---------- */
void updateDisplay() {
  display.clearDisplay();
  display.setTextSize(2);  // Minimum font size requirement
  display.setCursor(0, 0);

  display.print("L:");
  display.println(leftSpeed);

  display.print("R:");
  display.println(rightSpeed);

  display.print("Bat:");
  display.print(batteryPercent);
  display.println("%");

  display.display();
}

/* ---------- Setup ---------- */
void setup() {
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(EN1, OUTPUT);

  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(EN2, OUTPUT);

  Wire.begin(I2C_ADDR);
  Wire.onReceive(receiveEvent);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.display();
}

/* ---------- Loop ---------- */
void loop() {

  // 1️⃣ Process new command
  if (newCommand) {
    if (cmdBuffer[0] == 0x01) {
      leftSpeed = (int)cmdBuffer[1] - 128;
      rightSpeed = (int)cmdBuffer[2] - 128;
    }
    newCommand = false;
  }

  // 2️⃣ Fast motor update (always)
  updateMotors();

  // 3️⃣ Battery every 500ms
  if (millis() - lastBatteryRead > 500) {
    readBattery();
    lastBatteryRead = millis();
  }

  // 4️⃣ Display every 500ms
  if (millis() - lastDisplayUpdate > 500) {
    updateDisplay();
    lastDisplayUpdate = millis();
  }
}