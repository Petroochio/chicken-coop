/******************************************************************************

******************************************************************************/

#include "TB6612_ESP8266.h"
#include <Arduino.h>

Motor::Motor(int In1pin, int In2pin, int PWMpin, int STBYpin)
{
  In1 = In1pin;
  In2 = In2pin;
  PWM = PWMpin;
  Standby = STBYpin;

  pinMode(In1, OUTPUT);
  pinMode(In2, OUTPUT);
  pinMode(PWMpin, OUTPUT);
  pinMode(Standby, OUTPUT);

  digitalWrite(PWMpin, HIGH);
}

void Motor::drive(int speed)
{
  digitalWrite(Standby, HIGH);
  if (speed>=0) fwd();
  else rev();
}

void Motor::fwd()
{
   digitalWrite(In1, HIGH);
   digitalWrite(In2, LOW);
}

void Motor::rev()
{
   digitalWrite(In1, LOW);
   digitalWrite(In2, HIGH);
}

void Motor::brake()
{
   digitalWrite(In1, HIGH);
   digitalWrite(In2, HIGH);
}

void Motor::standby()
{
   digitalWrite(Standby, LOW);
}

// void forward(Motor motor1, Motor motor2, int speed)
// {
// 	motor1.drive(speed);
// 	motor2.drive(speed);
// }
// void forward(Motor motor1, Motor motor2)
// {
// 	motor1.drive(DEFAULTSPEED);
// 	motor2.drive(DEFAULTSPEED);
// }


// void back(Motor motor1, Motor motor2, int speed)
// {
// 	int temp = abs(speed);
// 	motor1.drive(-temp);
// 	motor2.drive(-temp);
// }
// void back(Motor motor1, Motor motor2)
// {
// 	motor1.drive(-DEFAULTSPEED);
// 	motor2.drive(-DEFAULTSPEED);
// }
// void left(Motor left, Motor right, int speed)
// {
// 	int temp = abs(speed)/2;
// 	left.drive(-temp);
// 	right.drive(temp);

// }


// void right(Motor left, Motor right, int speed)
// {
// 	int temp = abs(speed)/2;
// 	left.drive(temp);
// 	right.drive(-temp);

// }
// void brake(Motor motor1, Motor motor2)
// {
// 	motor1.brake();
// 	motor2.brake();
// }
