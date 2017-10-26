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

#ifndef THRUSTER-COMMANDER
#define THRUSTER-COMMANDER

#include <Arduino.h>

// HARDWARE PIN DEFINITIONS
#define INPUT_L     A3  // Left In
#define INPUT_R     A2  // Right In
#define INPUT_SPD   A3  // Speed
#define INPUT_STR   A1  // Steering
#define SWITCH      4
#define PWM_L       5
#define PWM_R       6
#define LED_L       8
#define LED_R       7
#define DETECT      0

// PWM GENERATION DEFINITIONS
#define PWM_FREQ    50                // Hz
#define PERIOD      1000000/PWM_FREQ  // us
#define PRESCALE    8                 // must match TCCR1B settings
#define CLOCK_FREQ  8000000ul         // Hz
#define CNT_PER_US  1                 // CLOCK_FREQ/PRESCALE/1000000 timer counts

// PWM OUTPUT CHARACTERISTICS
#define PWM_MAX     2000              // us
#define PWM_MIN     1000              // us
#define PWM_NEUTRAL 1500              // us
#define STEER_MAX   400               // us
#define HALF_RANGE  500               // us
#define DEADZONE    25                // us
#define POT_OFFSET  -12               // adc counts

// LOW-PASS FILTER
#define UPDATE_FREQ 20.0f             // Hz
#define CUTOFF_FREQ 0.5f              // Hz


#endif
