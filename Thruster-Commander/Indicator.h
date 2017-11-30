/* Blue Robotics Thruster Commander Firmware
-----------------------------------------------------

Title: Blue Robotics Thruster Commander Firmware - Indicator

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

#ifndef INDICATOR
#define INDICATOR

#include <Arduino.h>

#define OC0A_PIN  8
#define OC0B_PIN  7

#define BLINK_1S  0b00000001  // 1 short blink
#define BLINK_2S  0b00000101  // 2 short blinks
#define BLINK_3S  0b00010101  // 3 short blinks
#define BLINK_4S  0b01010101  // 4 short blinks
#define BLINK_1L  0b00000111  // 1 long blink
#define BLINK_2L  0b01110111  // 2 long blinks

#define BLINK_S   0b0101010101010101  // constant short blinking
#define BLINK_L   0b0111011101110111  // constant long blinking

void initializeLEDs();
void writeDimmer(int pin, int pulsewidth);
void writeBlinker(uint16_t ptrn);
void setDimmerBits();
void setBlinkerBits();

#endif
