#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

/* ---------- OLED ---------- */
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

/* ---------- I2C ---------- */
#define I2C_ADDR 0x08
#define BUFFER_SIZE 32

/* ---------- Buffer ---------- */
char messageBuffer[BUFFER_SIZE];
volatile bool newMessage = false;

/* ---------- I2C Receive Event ---------- */
void receiveEvent(int howMany) {
  int i = 0;

  while (Wire.available() && i < BUFFER_SIZE - 1) {
    messageBuffer[i++] = Wire.read();
  }

  messageBuffer[i] = '\0';  // Null terminate
  newMessage = true;
}

void setup() {
  Wire.begin(I2C_ADDR);          // Join I2C as slave
  Wire.onReceive(receiveEvent);  // Register receive event

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.display();
}

void loop() {
  if (newMessage) {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println(messageBuffer);
    display.display();
    newMessage = false;
  }
}