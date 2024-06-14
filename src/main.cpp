// Include the AccelStepper Library
#include <AccelStepper.h>
#include <Wire.h>
#include <uRTCLib.h>
#include <NewPing.h>
#include <DHT.h>

unsigned char low_data[8] = {0};
unsigned char high_data[12] = {0};

#define NO_TOUCH       0xFE
#define THRESHOLD      100
#define ATTINY1_HIGH_ADDR   0x78
#define ATTINY2_LOW_ADDR   0x77


#define DHT11_PIN 13

DHT dht11(DHT11_PIN, DHT11);

// Define pin connections
const int dirPin = 6;
const int stepPin = 5;
int day, hour, minute, dayOfWeek, w_lv_value, food_lv;

// Define motor interface type
#define motorInterfaceType 1

uRTCLib rtc(0x68);
NewPing sonar(7, 8, 20);

void getHigh12SectionValue(void)
{
  memset(high_data, 0, sizeof(high_data));
  Wire.requestFrom(ATTINY1_HIGH_ADDR, 12);
  while (12 != Wire.available());

  for (int i = 0; i < 12; i++) {
    high_data[i] = Wire.read();
  }
  delay(10);
}

void getLow8SectionValue(void)
{
  memset(low_data, 0, sizeof(low_data));
  Wire.requestFrom(ATTINY2_LOW_ADDR, 8);
  while (8 != Wire.available());

  for (int i = 0; i < 8 ; i++) {
    low_data[i] = Wire.read(); // receive a byte as character
  }
  delay(10);
}

void check()
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

    for (int i = 0 ; i < 8; i++) {
      if (low_data[i] > THRESHOLD) {
        touch_val |= 1 << i;

      }
    }
    for (int i = 0 ; i < 12; i++) {
      if (high_data[i] > THRESHOLD) {
        touch_val |= (uint32_t)1 << (8 + i);
      }
    }

    while (touch_val & 0x01)
    {
      trig_section++;
      touch_val >>= 1;
    }
    Serial.print("water level = ");
    Serial.print(trig_section * 5);
    Serial.println("% ");
    Serial.println(" ");
    Serial.println("*********************************************************");
    delay(1000);
}

// Creates an instance
AccelStepper myStepper(motorInterfaceType, stepPin, dirPin);

void moveStepper(){
  myStepper.moveTo(-9600); // Set desired move: 800 steps (in quater-step resolution that's one rotation)
  myStepper.runToPosition(); // Moves the motor to target position w/ acceleration/ deceleration and it blocks until is in position
}
void moveStepperBack(){
  myStepper.moveTo(0);
  myStepper.runToPosition();
}

void readFood(int &food_lv){
  int val;
  val = sonar.ping_cm();
  food_lv = map(val,0,20,100,0);
  Serial.println("Food Level");
  Serial.print(food_lv);
  Serial.print("%");
}

void readTempHumi(float &humi, float &temp)
{
    humi = dht11.readHumidity();
    temp = dht11.readTemperature();
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

void setup() {
	// set the maximum speed, acceleration factor,
	// initial speed and the target position
  Serial.begin(115200);
  URTCLIB_WIRE.begin();
  Wire.begin();
  dht11.begin();
  rtc.set(0, 6, 14, 3, 28, 2, 24);
	myStepper.setMaxSpeed(1000);
	myStepper.setAcceleration(50);
	myStepper.setSpeed(200);
  myStepper.setCurrentPosition(0);
}

void loop() {

  moveStepper();
  rtcRead(dayOfWeek, day, hour, minute);
  check();
  readFood(food_lv);
  moveStepperBack();

}