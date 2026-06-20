#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include "BluetoothSerial.h"

BluetoothSerial SerialBT;
Adafruit_MPU6050 mpu;

// Flex Sensors
#define FLEX1 33
#define FLEX2 32
#define FLEX3 35
#define FLEX4 34

// Relay Pins
#define LIGHT_RELAY 25
#define MOTOR_RELAY 26

// Threshold
#define FLEX_THRESHOLD 2000
#define DEAD_ZONE 1.5
#define FALL_THRESHOLD 25.0

void setup()
{
  Serial.begin(115200);
  SerialBT.begin("SenseSpeak");

  Wire.begin();

  if (!mpu.begin())
  {
    Serial.println("MPU6050 Failed");
    while (1);
  }

  pinMode(LIGHT_RELAY, OUTPUT);
  pinMode(MOTOR_RELAY, OUTPUT);

  digitalWrite(LIGHT_RELAY, HIGH);
  digitalWrite(MOTOR_RELAY, HIGH);

  Serial.println("System Ready");
}

void loop()
{
  // FLEX SENSOR READINGS
  int flex1 = analogRead(FLEX1);
  int flex2 = analogRead(FLEX2);
  int flex3 = analogRead(FLEX3);
  int flex4 = analogRead(FLEX4);

  int m1 = (flex1 > FLEX_THRESHOLD) ? 1 : 0;
  int m2 = (flex2 > FLEX_THRESHOLD) ? 1 : 0;
  int m3 = (flex3 > FLEX_THRESHOLD) ? 1 : 0;
  int m4 = (flex4 > FLEX_THRESHOLD) ? 1 : 0;

  // MPU6050
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  float accelX = fabs(a.acceleration.x) > DEAD_ZONE ? a.acceleration.x : 0;
  float accelY = fabs(a.acceleration.y) > DEAD_ZONE ? a.acceleration.y : 0;

  int m5 = (accelX > 0) ? 1 : 0;
  int m6 = (accelY > 0) ? 1 : 0;

  // BLUETOOTH DATA TO APP
  SerialBT.print(m1); SerialBT.print("|");
  SerialBT.print(m2); SerialBT.print("|");
  SerialBT.print(m3); SerialBT.print("|");
  SerialBT.print(m4); SerialBT.print("|");
  SerialBT.print(m5); SerialBT.print("|");
  SerialBT.println(m6);

  // SERIAL MONITOR
  Serial.print("Gesture: ");
  Serial.print(m1); Serial.print("|");
  Serial.print(m2); Serial.print("|");
  Serial.print(m3); Serial.print("|");
  Serial.print(m4); Serial.print("|");
  Serial.print(m5); Serial.print("|");
  Serial.println(m6);

  // MOTOR ON
  if (m1 == 0 && m2 == 0 && m3 == 1 && m4 == 1)
  {
    digitalWrite(MOTOR_RELAY, LOW);
    Serial.println("Motor ON");
    SerialBT.println("MOTOR_ON");
  }

  // MOTOR OFF
  else if (m1 == 1 && m2 == 1 && m3 == 0 && m4 == 0)
  {
    digitalWrite(MOTOR_RELAY, HIGH);
    Serial.println("Motor OFF");
    SerialBT.println("MOTOR_OFF");
  }

  // LIGHT ON
  if (m1 == 0 && m2 == 1 && m3 == 1 && m4 == 1)
  {
    digitalWrite(LIGHT_RELAY, LOW);
    Serial.println("Light ON");
    SerialBT.println("LIGHT_ON");
  }

  // LIGHT OFF
  else if (m1 == 1 && m2 == 0 && m3 == 1 && m4 == 1)
  {
    digitalWrite(LIGHT_RELAY, HIGH);
    Serial.println("Light OFF");
    SerialBT.println("LIGHT_OFF");
  }

  // EMERGENCY GESTURE
  if (m1 == 0 && m2 == 0 && m3 == 0 && m4 == 0)
  {
    Serial.println("EMERGENCY");
    SerialBT.println("EMERGENCY");
  }

  // HELP GESTURE
  if (m1 == 1 && m2 == 1 && m3 == 1 && m4 == 1)
  {
    Serial.println("HELP");
    SerialBT.println("HELP");
  }

  // FALL DETECTION
  float totalAcc =
      sqrt(
          a.acceleration.x * a.acceleration.x +
          a.acceleration.y * a.acceleration.y +
          a.acceleration.z * a.acceleration.z);

  if (totalAcc > FALL_THRESHOLD)
  {
    Serial.println("FALL DETECTED");
    SerialBT.println("FALL_DETECTED");
    delay(2000);
  }

  delay(300);
}
this is my code how to upload in githb