/* Blue Robotics Thruster Commander Firmware
-----------------------------------------------------
 
Title: Blue Robotics Thruster Commander Firmware

Description: This code is the default firmware for the Blue Robotics
Thruster Commander, which provides a simple interface to control a 
bidirectional speed controller with PWM signals. It can be used to test 
motors or to control simple vehicles like a kayak.

The code is designed for the ATtiny44 microcontroller and can be compiled and 
uploaded via the Arduino 1.0+ software.

-------------------------------
The MIT License (MIT)

Copyright (c) 2017 Blue Robotics Inc.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
-------------------------------*/

#include <Servo.h>

// HARDWARE PIN DEFINITIONS
#define INPUT_INT   A1
#define INPUT_EXT0  A2
#define INPUT_EXT1  A3
#define PWM_0       6
#define PWM_1       5
#define LED_INT     4
#define LED_EXT0    8
#define LED_EXT1    7
#define DETECT      0

Servo pwm0, pwm1;

bool ext0IsConnected, ext1IsConnected;
int pwmOut0, pwmOut1;

void setup() {
  // Set up pin modes
  pinMode(INPUT_INT,INPUT);
  pinMode(INPUT_EXT0,INPUT);
  pinMode(INPUT_EXT1,INPUT);
  pinMode(DETECT,OUTPUT);
  pinMode(LED_INT,OUTPUT);
  pinMode(LED_EXT0,OUTPUT);
  pinMode(LED_EXT1,OUTPUT);

  // Set up PWM outputs
  pwm0.attach(PWM_0);
  pwm1.attach(PWM_1);
  pwm0.write(1500);
  pwm1.write(1500);

  // Detect what's connected
  detect();

  // Delay to allow ESCs to initialize
  digitalWrite(LED_INT,HIGH);
  delay(200);
  digitalWrite(LED_INT,LOW);
  delay(200);
  digitalWrite(LED_INT,HIGH);
  delay(200);
  digitalWrite(LED_INT,LOW);
}

void loop() {
  int pwmInt, pwmExt0, pwmExt1, pwmBase, pwmSteer;
  int input_int, input_ext0, input_ext1;

  // Read raw inputs
  input_int = analogRead(INPUT_INT);
  input_ext0 = analogRead(INPUT_EXT0);
  input_ext1 = analogRead(INPUT_EXT1);

  // Map standard inputs to 1000-2000 µs range
  pwmInt  = map(input_int,0,1023,1000,2000);
  pwmExt0 = map(input_ext0,0,1023,1000,2000);
  pwmExt1 = map(input_ext1,0,1023,1000,2000);

  // Map mixed inputs for mixing mode
  bool isMixMode = input_int > 100;
  int steerMax = map(input_int,100,1023,0,400);
  pwmSteer = map(input_ext1,0,1023,-steerMax,steerMax);
  pwmBase = map(input_ext0,0,1023,1000,2000);
 
  // Logic:
  // If both external inputs are connected:
  //   If the internal input is low, map outputs independently
  //   If the internal input is 100-1023, map to mixed steering mode proportional to internal input 
  // If one external input is connected, map it to both outputs
  // If neither is connected, map the internal input to both outputs
  if ( ext0IsConnected && ext1IsConnected ) {
    if ( isMixMode ) {
      pwmOut0 = pwmBase+pwmSteer;
      pwmOut1 = pwmBase-pwmSteer;
    } else {
      pwmOut0 = pwmExt0;
      pwmOut1 = pwmExt1;
    }
  } else {
    if ( ext0IsConnected ) {
      pwmOut0 = pwmExt0;
      pwmOut1 = pwmExt0;
    } else if ( ext1IsConnected ) {
      pwmOut0 = pwmExt1;
      pwmOut1 = pwmExt1;
    } else {
      pwmOut0 = pwmInt;
      pwmOut1 = pwmInt;
    }
  }

  pwm0.write(pwmOut0);
  pwm1.write(pwmOut1);

  setLEDs();

  delay(50); // About 20 Hz update rate
}

void detect() {
  // Detect what's connected by driving lines through 1M resistors
  int a0, b0, a1, b1;

  // Drive both inputs low, wait, and log values
  digitalWrite(DETECT,LOW);
  delay(100);
  a0 = analogRead(INPUT_EXT0);
  a1 = analogRead(INPUT_EXT1);

  // Drive both inputs high, wait, and log values
  digitalWrite(DETECT,HIGH);
  delay(100);
  b0 = analogRead(INPUT_EXT0);
  b1 = analogRead(INPUT_EXT1);

  // If inputs follow, potentiometer is disconnected, otherwise it's connected
  if ( a0 < 5 && b0 > 1018 ) {
    ext0IsConnected = false;
  } else {
    ext0IsConnected = true;
  }

  if ( a1 < 5 && b1 > 1018 ) {
    ext1IsConnected = false;
  } else {
    ext1IsConnected = true;
  }

  digitalWrite(DETECT,LOW);
}

void setLEDs() {
  int period0, period1, periodAux;

  // Set to solid at 1500 and flash faster in each direction
  if ( pwmOut0 > 1525 ) {
    period0 = map(pwmOut0,1525,2000,2000,200);
  } else if ( pwmOut0 < 1475 ) {
    period0 = map(pwmOut0,1475,1000,2000,200);
  } else {
    period0 = 0xffff;
  }

  if ( pwmOut1 > 1525 ) {
    period1 = map(pwmOut1,1525,2000,2000,200);
  } else if ( pwmOut0 < 1475 ) {
    period1 = map(pwmOut1,1475,1000,2000,200);
  } else {
    period1 = 0xffff;
  }

  // Implement flashing
  digitalWrite(LED_EXT0,(millis()/(period0/2))%2);
  digitalWrite(LED_EXT1,(millis()/(period1/2))%2);
  //digitalWrite(LED_INT, (millis()/(periodAux/2))%2);
}

