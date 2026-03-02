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
#define L_IN1 4
#define L_IN2 5
#define L_PWM 6

#define R_IN1 7
#define R_IN2 8
#define R_PWM 9

/* ---------- Battery ---------- */
#define BATTERY_PIN A0
#define VOLTAGE_SCALE 1.0f

/* ---------- State ---------- */
volatile int8_t leftCmd = 0;
volatile int8_t rightCmd = 0;

float batteryVoltage = 0;
uint8_t batteryPercent = 0;

/* ---------- Timers ---------- */
uint32_t lastDisplay = 0;
uint32_t lastBattery = 0;

const uint16_t DISPLAY_INTERVAL = 100;
const uint16_t BATTERY_INTERVAL = 500;

/* ========================================================= */
/* ================= MOTOR CONTROL ========================= */
/* ========================================================= */

void setMotor(int pwmPin,int in1,int in2,int value)
{
    value = constrain(value,-255,255);

    if(value > 0){
        digitalWrite(in1,HIGH);
        digitalWrite(in2,LOW);
        analogWrite(pwmPin,value);
    }
    else if(value < 0){
        digitalWrite(in1,LOW);
        digitalWrite(in2,HIGH);
        analogWrite(pwmPin,-value);
    }
    else{
        digitalWrite(in1,LOW);
        digitalWrite(in2,LOW);
        analogWrite(pwmPin,0);
    }
}

/* ========================================================= */
/* ===================== I2C EVENTS ======================== */
/* ========================================================= */

void onReceive(int count)
{
    if(count >= 2){
        leftCmd  = (int8_t)Wire.read();
        rightCmd = (int8_t)Wire.read();
    }
}

void onRequest()
{
    uint16_t mv = (uint16_t)(batteryVoltage * 1000);
    Wire.write((uint8_t*)&mv,2);
}

/* ========================================================= */
/* ================= BATTERY TASK ========================== */
/* ========================================================= */

void updateBattery()
{
    int raw = analogRead(BATTERY_PIN);

    batteryVoltage =
        raw * (5.0f / 1023.0f) * VOLTAGE_SCALE;

    float pct = (batteryVoltage - 3.3f) / (4.2f - 3.3f);
    pct = constrain(pct,0,1);

    batteryPercent = pct * 100;
}

/* ========================================================= */
/* ================= DISPLAY HELPERS ======================= */
/* ========================================================= */

void drawMotorBar(int y,int value,const char* label)
{
    int center = 64;
    int maxWidth = 50;

    int mag = map(abs(value),0,255,0,maxWidth);

    display.setCursor(0,y);
    display.print(label);

    // center marker
    display.drawLine(center,y+8,center,y+14,SSD1306_WHITE);

    if(value > 0)
        display.fillRect(center,y+9,mag,4,SSD1306_WHITE);
    else if(value < 0)
        display.fillRect(center-mag,y+9,mag,4,SSD1306_WHITE);
}

/* ========================================================= */
/* ================= DISPLAY TASK ========================== */
/* ========================================================= */

void updateDisplay()
{
    display.clearDisplay();

    // ---- BATTERY ----
    display.setTextSize(2);
    display.setCursor(34,4);   // moved down slightly
    display.print(batteryPercent);
    display.print("%");

    // ---- MOTOR BARS ----
    display.setTextSize(1);

    drawMotorBar(28,leftCmd,"L");
    drawMotorBar(46,rightCmd,"R");

    display.display();
}

/* ========================================================= */
/* ========================= SETUP ========================= */
/* ========================================================= */

void setup()
{
    pinMode(L_IN1,OUTPUT);
    pinMode(L_IN2,OUTPUT);
    pinMode(R_IN1,OUTPUT);
    pinMode(R_IN2,OUTPUT);
    pinMode(L_PWM,OUTPUT);
    pinMode(R_PWM,OUTPUT);

    /* I2C FIRST */
    Wire.begin(I2C_ADDR);
    Wire.onReceive(onReceive);
    Wire.onRequest(onRequest);

    /* OLED AFTER I2C */
    display.begin(SSD1306_SWITCHCAPVCC,0x3C);
    display.setTextColor(SSD1306_WHITE);   // ⭐ REQUIRED
    display.clearDisplay();
    display.display();
}

/* ========================================================= */
/* ========================== LOOP ========================= */
/* ========================================================= */

void loop()
{
    setMotor(L_PWM,L_IN1,L_IN2,leftCmd);
    setMotor(R_PWM,R_IN1,R_IN2,rightCmd);

    uint32_t now = millis();

    if(now - lastBattery >= BATTERY_INTERVAL){
        lastBattery = now;
        updateBattery();
    }

    if(now - lastDisplay >= DISPLAY_INTERVAL){
        lastDisplay = now;
        updateDisplay();
    }
}