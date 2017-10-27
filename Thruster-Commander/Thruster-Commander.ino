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

#include "Thruster-Commander.h"

#include "Servo-Driver.h"
#include "Blinker.h"
#include "LPFilter.h"

// Global Variable Declaration
bool      inLIsConnected, inRIsConnected, inSPDIsConnected, inSTRIsConnected;
int       pwmOutL, pwmOutR;
blinker_t leds[2];    // 0:left, 1:right
LPFilter  filterL, filterR;


void setup() {
  // Set up pin modes
  pinMode(INPUT_L,INPUT);
  pinMode(INPUT_R,INPUT);
  pinMode(INPUT_SPD,INPUT);
  pinMode(INPUT_STR,INPUT);
  pinMode(SWITCH,INPUT);
  pinMode(DETECT,OUTPUT);
  pinMode(LED_L,OUTPUT);
  pinMode(LED_R,OUTPUT);
  pinMode(PWM_L,OUTPUT);
  pinMode(PWM_R,OUTPUT);

  // Initialize motor controllers
  initializePWMController();
  writePWML(PWM_NEUTRAL);
  writePWMR(PWM_NEUTRAL);

  // Initialize low-pass filters
  filterL = LPFilter(1.0f/UPDATE_FREQ, 1.0f/CUTOFF_FREQ, PWM_NEUTRAL);
  filterR = LPFilter(1.0f/UPDATE_FREQ, 1.0f/CUTOFF_FREQ, PWM_NEUTRAL);

  // Initialize LEDs
  initializeLEDs(leds);

  // Detect what's connected
  detect();

  // Delay to allow ESCs to initialize
  delay(600);
}

void loop() {
  int pwmL, pwmR, pwmSPD, pwmSTR;
  int inputL, inputR, inputSPD, inputSTR, inputSWITCH;

  // Read switch
  inputSWITCH = digitalRead(SWITCH);

  // Read raw inputs
  inputL   = analogRead(INPUT_L);
  inputR   = analogRead(INPUT_R);
  inputSPD = analogRead(INPUT_SPD);
  inputSTR = analogRead(INPUT_STR);

  // Map standard inputs to 1000-2000 µs range
  pwmL   = map(inputL   - POT_OFFSET,0,1023,PWM_MIN,PWM_MAX);
  pwmR   = map(inputR   - POT_OFFSET,0,1023,PWM_MIN,PWM_MAX);
  pwmSPD = map(inputSPD - POT_OFFSET,0,1023,PWM_MIN,PWM_MAX);

  // Map steering to +/- steering range
  pwmSTR = map(inputSTR - POT_OFFSET,0,1023,-STEER_MAX,STEER_MAX);

  // Logic:
  // If SWITCH is pulled low (enabled):
  //   If both L and R are connected:
  //     L controls pwmOutL, R controls pwmOutR
  //   If both SPD and STR are connected:
  //     Mix SPD and STR to control pwmOutL and pwmOutR
  //   If only L is connected:
  //     L controls both pwmOutL and pwmOutR
  //   If only R is connected:
  //     R controls both pwmOutL and pwmOutR
  if ( inputSWITCH == LOW ) {
    if ( inLIsConnected && inRIsConnected ) {
      pwmOutL = pwmL;
      pwmOutR = pwmR;
    } else if ( inSPDIsConnected && inSTRIsConnected ) {
      pwmOutL = constrain(pwmSPD+pwmSTR,PWM_MIN,PWM_MAX);
      pwmOutR = constrain(pwmSPD-pwmSTR,PWM_MIN,PWM_MAX);
    } else if ( inLIsConnected ) {
      pwmOutL = pwmL;
      pwmOutR = pwmL;
    } else if ( inRIsConnected ) {
      pwmOutL = pwmR;
      pwmOutR = pwmR;
    }
  } else {
    pwmOutL = PWM_NEUTRAL;
    pwmOutR = PWM_NEUTRAL;
  }

  // Run pwm outputs through filters
  pwmOutL = filterL.step(pwmOutL);
  pwmOutR = filterR.step(pwmOutR);

  // Set pwm outputs
  writePWML(pwmOutL);
  writePWMR(pwmOutR);

  setLEDs(leds, pwmOutL, pwmOutR);

  delay(1.0f/UPDATE_FREQ);   // set update rate
}


void detect() {
  // Detect what's connected by driving lines through 100k resistors
  int inL[2], inR[2], inSPD[2], inSTR[2];   // 0:low, 1:high

  // Drive both inputs low, wait, and log values
  digitalWrite(DETECT,LOW);
  delay(100);
  inL[0]   = analogRead(INPUT_L);
  inR[0]   = analogRead(INPUT_R);
  inSPD[0] = analogRead(INPUT_SPD);
  inSTR[0] = analogRead(INPUT_STR);

  // Drive both inputs high, wait, and log values
  digitalWrite(DETECT,HIGH);
  delay(100);
  inL[1]   = analogRead(INPUT_L);
  inR[1]   = analogRead(INPUT_R);
  inSPD[1] = analogRead(INPUT_SPD);
  inSTR[1] = analogRead(INPUT_STR);

  // If inputs follow, potentiometer is disconnected, otherwise it's connected
  if ( inL[0] < 5 && inL[1] > 1018 ) {
    inLIsConnected = false;
  } else {
    inLIsConnected = true;
  }

  if ( inR[0] < 5 && inR[1] > 1018 ) {
    inRIsConnected = false;
  } else {
    inRIsConnected = true;
  }

  if ( inSPD[0] < 5 && inSPD[1] > 1018 ) {
    inSPDIsConnected = false;
  } else {
    inSPDIsConnected = true;
  }

  if ( inSTR[0] < 5 && inSTR[1] > 1018 ) {
    inSTRIsConnected = false;
  } else {
    inSTRIsConnected = true;
  }

  digitalWrite(DETECT,LOW);
}
