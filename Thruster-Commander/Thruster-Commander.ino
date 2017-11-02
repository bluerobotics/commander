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
#include "Indicator.h"
#include "LPFilter.h"

// Global Variable Declaration
bool      inLIsConnected, inRIsConnected, inSPDIsConnected, inSTRIsConnected;
LPFilter  filterL, filterR;
uint32_t  schedulePWM, scheduleDetect;


void setup() {
  // Set up pin modes
  pinMode(INPUT_L,INPUT);
  pinMode(INPUT_R,INPUT);
  pinMode(INPUT_SPD,INPUT);
  pinMode(INPUT_STR,INPUT);
  pinMode(SWITCH,INPUT);
  pinMode(DETECT,OUTPUT);
  pinMode(PWM_L,OUTPUT);
  pinMode(PWM_R,OUTPUT);
  pinMode(LED_L,OUTPUT);
  pinMode(LED_R,OUTPUT);

  // Initialize motor controllers
  initializePWMController();
  writePWM(PWM_L, PWM_NEUTRAL);
  writePWM(PWM_R, PWM_NEUTRAL);

  // Initialize LEDs
  initializeLEDs();
  writeBlinker(BLINK_S);

  // Detect what's connected
  detect();

  // Delay to allow ESCs to initialize
  delay(600);

  // Schedule the next pwm, detect times
  schedulePWM    = millis();
  scheduleDetect = millis() + DETECT_DT*1000.0f;
}

void loop() {
  // Update PWM signals
  if (millis() > schedulePWM) {
    int pwmL, pwmR, pwmSPD, pwmSTR;
    int pwmOutL, pwmOutR;
    int inputL, inputR, inputSPD, inputSTR, inputSWITCH;
    uint16_t errorPtrn = 0;

    // Schedule the next PWM time
    schedulePWM = millis() + FILTER_DT*1000;

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
      } else {
        pwmOutL   = PWM_NEUTRAL;
        pwmOutR   = PWM_NEUTRAL;
        errorPtrn = BLINK_2S;
      }
    } else {
      pwmOutL   = PWM_NEUTRAL;
      pwmOutR   = PWM_NEUTRAL;
      errorPtrn = BLINK_1L;
    }

    // Run pwm outputs through filters
    pwmOutL = filterL.step(pwmOutL);
    pwmOutR = filterR.step(pwmOutR);

    // Set pwm outputs
    writePWM(PWM_L, pwmOutL);
    writePWM(PWM_R, pwmOutR);

    // Set LEDs
    if (errorPtrn == 0) {
      // No errors, display dimmer value
      writeDimmer(LED_L, pwmOutL);
      writeDimmer(LED_R, pwmOutR);
    } else {
      // display error pattern
      writeBlinker(errorPtrn);
    }

    return;
  }

  // Update detect
  if (millis() > scheduleDetect) {
    // Schedule the next detect time
    scheduleDetect = millis() + DETECT_DT*1000;

    // Re-run detect()
    detect();

    return;
  }
}


void detect() {
  // Detect what's connected by driving lines through 100k resistors
  int inL[2], inR[2], inSPD[2], inSTR[2];   // 0:low, 1:high

  // Drive both inputs high, wait, and log values
  digitalWrite(DETECT,HIGH);
  delay(10);
  inL[1]   = analogRead(INPUT_L);
  inR[1]   = analogRead(INPUT_R);
  inSPD[1] = analogRead(INPUT_SPD);
  inSTR[1] = analogRead(INPUT_STR);

  // Drive both inputs low, wait, and log values
  digitalWrite(DETECT,LOW);
  delay(10);
  inL[0]   = analogRead(INPUT_L);
  inR[0]   = analogRead(INPUT_R);
  inSPD[0] = analogRead(INPUT_SPD);
  inSTR[0] = analogRead(INPUT_STR);

  // If inputs follow, potentiometer is disconnected, otherwise it's connected
  inLIsConnected   = !( inL[0]   < DETECT_LOW && inL[1]   > DETECT_HIGH );
  inRIsConnected   = !( inR[0]   < DETECT_LOW && inR[1]   > DETECT_HIGH );
  inSPDIsConnected = !( inSPD[0] < DETECT_LOW && inSPD[1] > DETECT_HIGH );
  inSTRIsConnected = !( inSTR[0] < DETECT_LOW && inSTR[1] > DETECT_HIGH );
}
