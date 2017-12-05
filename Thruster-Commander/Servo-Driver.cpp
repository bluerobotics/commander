/* Blue Robotics Thruster Commander Firmware
-----------------------------------------------------

Title: Blue Robotics Thruster Commander Firmware - Servo Driver

Description: This code is the default firmware for the Blue Robotics
Thruster Commander, which provides a simple interface to control a
bidirectional speed controller with PWM signals. It can be used to test
motors or to control simple vehicles like a kayak.

The code is designed for the ATtiny84 microcontroller and can be compiled and
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

#include "Servo-Driver.h"
#include "Thruster-Commander.h"

void writePWM(int pin, int pulsewidth) {
  // Stop interrupts while changing pwm settings
  cli();

  // Constrain pulsewidth to pwm range if > 0, otherwise set to neutral
  pulsewidth = (pulsewidth > 0) ? constrain(pulsewidth, PWM_MIN, PWM_MAX)
               : PWM_NEUTRAL;

  if (pin == OC1A_PIN) {
    // Set timer1 Output Compare Register A
    // Set shut-off counter value to get pulsewidth us pulse
    OCR1A = (pulsewidth * CNT_PER_US) - 1;
  } else if (pin == OC1B_PIN) {
    // Set timer1 Output Compare Register B
    // Set shut-off counter value to get pulsewidth us pulse
    OCR1B = (pulsewidth * CNT_PER_US) - 1;
  }

  // Done setting timers -> allow interrupts again
  sei();
}

void initializePWMController() {
  // Stop interrupts while changing timer settings
  cli();

  // Set timer1 control register A/B/C
  TCCR1A  = 0;
  TCCR1B  = 0;
  TCCR1C  = 0;

  // Set non-inverting Fast PWM mode on A and B
  TCCR1A |= (1 << COM1A1);
  TCCR1A |= (1 << COM1B1);
  // Set Fast PWM mode (compare to ICR1)
  TCCR1A |= (1 << WGM11);
  TCCR1B |= (1 << WGM12);
  TCCR1B |= (1 << WGM13);
  // Set timer1 clock source to prescaler 8
  TCCR1B |= (1 << CS11);

  // Set timer1 Input Capture Register
  // Set end counter value to get a PWM_FREQ Hz timer
  ICR1    = (PERIOD  * CNT_PER_US) - 1;

  // Done setting timers -> allow interrupts again
  sei();
}
