/* Blue Robotics Thruster Commander Firmware
-----------------------------------------------------

Title: Blue Robotics Thruster Commander Firmware - Blinker

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

#include "Blinker.h"
#include "Thruster-Commander.h"

int pwmToBlinkPeriod(int pulsewidth) {
  int period;
  // Constrain pulsewidth
  pulsewidth = constrain(pulsewidth,PWM_MIN,PWM_MAX);
  // Set to solid at 1500 and flash faster in each direction
  int blinkspeed = abs(pulsewidth - PWM_NEUTRAL);
  if ( blinkspeed > DEADZONE ) {
    period = map(blinkspeed,DEADZONE,HALF_RANGE,2000,200);
  } else {
    period = 0;
  }
  return period;
}

void initializeLEDs(blinker_t leds[]) {
  leds[0].pin = LED_L;
  leds[1].pin = LED_R;

  for ( int i = 0; i < 2; i++) {
    leds[i].lastblink = 0;
    leds[i].period    = 0;
    leds[i].state     = 0;
  }
}

void setLEDs(blinker_t leds[], int pwmOutL, int pwmOutR) {
  // Update LED periods
  leds[0].period = pwmToBlinkPeriod(pwmOutL); // left
  leds[1].period = pwmToBlinkPeriod(pwmOutR); // right

  // Blink each LED if necessary
  for(int i=0; i<2; i++) {
    if ( leds[i].period > 0 ) {
      if ( millis() > leds[i].lastblink + leds[i].period/2 ) {
        leds[i].lastblink = millis();
        leds[i].state     = !leds[i].state;
        digitalWrite(leds[i].pin, leds[i].state);
      }
    } else {
      if ( !leds[i].state ) {
        leds[i].state = true;
        digitalWrite(leds[i].pin, HIGH);
      }
    }
  }
}
