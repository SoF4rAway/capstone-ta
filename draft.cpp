// Library Import
#include <Arduino.h>
#include <AccelStepper.h>
#include <Wire.h>
#include <uRTCLib.h>
#include <DHT.h>
#include <NewPing.h>
#include <Servo.h>

// Pin Function Declaration:
#define servoPin 11
#define DHT11_PIN 13
#define SOL_1 3   // Cleaning Water
#define SOL_2 4   // Drinking Water
#define WTR_PMP 9 // Disinfectant Sprayer
#define TRG_PIN 7
#define ECH_PIN 8
#define motorInterfaceType 1
#define NO_TOUCH 0xFE
#define THRESHOLD 100
#define ATTINY1_HIGH_ADDR 0x78
#define ATTINY2_LOW_ADDR 0x77
#define disinfection_day 3
#define humi_threshold 90
#define cleaning_hour 16
#define MAX_DISTANCE 20
#define fnb_thres 30

const int dirPin = 6;
const int stepPin = 5;

// Obj Declaration:
uRTCLib rtc(0x68);
DHT dht22(DHT11_PIN, DHT22);
AccelStepper stepper(motorInterfaceType, stepPin, dirPin);
NewPing sonar(TRG_PIN, ECH_PIN, MAX_DISTANCE);
Servo servo;

// Memory Declaration
unsigned char low_data[8] = {0};
unsigned char high_data[12] = {0};
int lastDay = -1;
int lastHourChecked = -1;
int lastMinuteChecked = -1;
int day, hour, minute, dayOfWeek, w_lv_value;
int food_lv;
float humi, temp;

void getHigh12SectionValue(void)
{
  memset(high_data, 0, sizeof(high_data));
  Wire.requestFrom(ATTINY1_HIGH_ADDR, 12);
  while (12 != Wire.available())
    ;

  for (int i = 0; i < 12; i++)
  {
    high_data[i] = Wire.read();
  }
  delay(10);
}

void getLow8SectionValue(void)
{
  memset(low_data, 0, sizeof(low_data));
  Wire.requestFrom(ATTINY2_LOW_ADDR, 8);
  while (8 != Wire.available())
    ;

  for (int i = 0; i < 8; i++)
  {
    low_data[i] = Wire.read(); // receive a byte as character
  }
  delay(10);
}

void w_lv(int &w_lv_value)
{
  int sensorvalue_min = 250;
  int sensorvalue_max = 255;
  int low_count = 0;
  int high_count = 0;
  uint32_t touch_val = 0;
  uint8_t trig_section = 0;
  low_count = 0;
  high_count = 0;
  getLow8SectionValue();
  getHigh12SectionValue();

  Serial.println("low 8 sections value = ");
  for (int i = 0; i < 8; i++)
  {
    Serial.print(low_data[i]);
    Serial.print(".");
    if (low_data[i] >= sensorvalue_min && low_data[i] <= sensorvalue_max)
    {
      low_count++;
    }
    if (low_count == 8)
    {
      Serial.print("      ");
      Serial.print("PASS");
    }
  }
  Serial.println("  ");
  Serial.println("  ");
  Serial.println("high 12 sections value = ");
  for (int i = 0; i < 12; i++)
  {
    Serial.print(high_data[i]);
    Serial.print(".");

    if (high_data[i] >= sensorvalue_min && high_data[i] <= sensorvalue_max)
    {
      high_count++;
    }
    if (high_count == 12)
    {
      Serial.print("      ");
      Serial.print("PASS");
    }
  }

  Serial.println("  ");
  Serial.println("  ");

  for (int i = 0; i < 8; i++)
  {
    if (low_data[i] > THRESHOLD)
    {
      touch_val |= 1 << i;
    }
  }
  for (int i = 0; i < 12; i++)
  {
    if (high_data[i] > THRESHOLD)
    {
      touch_val |= (uint32_t)1 << (8 + i);
    }
  }

  while (touch_val & 0x01)
  {
    trig_section++;
    touch_val >>= 1;
  }
  w_lv_value = trig_section * 5;
  Serial.print("water level = ");
  Serial.print(w_lv_value);
  Serial.println("% ");
  Serial.println(" ");
  Serial.println("*********************************************************");
  delay(1000);
}

void moveStepper()
{
  stepper.moveTo(800);     // Set desired move: 800 steps (in quater-step resolution that's one rotation)
  stepper.runToPosition(); // Moves the motor to target position w/ acceleration/ deceleration and it blocks until is in position
}
void moveStepperBack()
{
  stepper.moveTo(0);
  stepper.runToPosition();
}

void readFood(int &food_lv)
{
  int val;
  val = sonar.ping_cm();
  food_lv = map(val, 0, 20, 100, 0);
  Serial.println("Food Level");
  Serial.print(food_lv);
  Serial.print("%");
}

void rtcRead(int &dayOfWeek, int &day, int &hour, int &minute)
{
  rtc.refresh();
  dayOfWeek = rtc.dayOfWeek();
  day = rtc.day();
  hour = rtc.hour();
  minute = rtc.minute();
  Serial.print("Current Date & Time: ");

  Serial.print(day);

  Serial.print(" (");
  Serial.print(dayOfWeek);
  Serial.print(") ");

  Serial.print(hour);
  Serial.print(':');
  Serial.print(minute);
  Serial.println();
}

void readTempHumi(float &humi, float &temp)
{
  humi = dht22.readHumidity();
  temp = dht22.readTemperature();
}

void setup()
{
  Serial.begin(115200);
  Wire.begin();

  URTCLIB_WIRE.begin();
  rtc.set(0, 6, 14, 3, 28, 2, 24);

  dht22.begin();

  pinMode(SOL_1, OUTPUT);
  digitalWrite(SOL_1, LOW);

  pinMode(SOL_2, OUTPUT);
  digitalWrite(SOL_2, LOW);

  pinMode(WTR_PMP, OUTPUT);
  digitalWrite(WTR_PMP, LOW);

  pinMode(TRG_PIN, OUTPUT);
  digitalWrite(TRG_PIN, LOW);

  pinMode(servoPin, OUTPUT);
  digitalWrite(servoPin, LOW);
  servo.attach(servoPin);

  pinMode(ECH_PIN, INPUT);

  stepper.setMaxSpeed(1000);
  stepper.setAcceleration(50);
  stepper.setSpeed(200);
  stepper.setCurrentPosition(0);
}

void loop()
{
  rtcRead(dayOfWeek, day, hour, minute);

  while (true)
  { // Continuously check the food level
    readFood(food_lv);
    Serial.println("*********************************************************");
    Serial.print('Food Level');
    Serial.print(food_lv);
    if (food_lv <= 25)
    {
      servo.write(90); // Open the servo
      while (food_lv <= 95)
      {
        food_lv = ((sonar.ping_cm()) / MAX_DISTANCE) * 100; // Update the food level
        if (food_lv > 95)
        {
          servo.write(0); // Close the servo
        }
      }
    }
    else
    {
      break; // Exit the loop
    }
  }

  while (true)
  { // Continuously check the water level
    w_lv(w_lv_value);
    Serial.print("water level = ");
    Serial.print(w_lv_value);
    Serial.print("% ");
    Serial.println(" ");

    if (w_lv_value <= 25)
    {
      digitalWrite(SOL_2, HIGH); // Open the solenoid valve
      while (w_lv_value <= 95)
      {
        w_lv(w_lv_value); // Update the water level
      }
      digitalWrite(SOL_2, LOW); // Close the solenoid valve
    }
    else
    {
      break;
    }
  }

  if (hour != lastHourChecked)
  {
    lastHourChecked = hour;
    if (hour == int(cleaning_hour) && day != lastDay)
    {
      lastDay = day; // Update the last day the operation was performed
      digitalWrite(SOL_1, HIGH);
      for (int i = 0; i <= 3; i++)
      {
        moveStepper();
        delay(500);
        moveStepperBack();
      }
      digitalWrite(SOL_1, LOW);
      if (dayOfWeek == disinfection_day)
      {
        readTempHumi(humi, temp);
        if (humi <= humi_threshold)
        {
          digitalWrite(WTR_PMP, HIGH);
          delay(3000);
          digitalWrite(WTR_PMP, LOW);
        }
      }
    }
  }
}