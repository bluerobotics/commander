/* Blue Robotics Thruster Commander Firmware
-----------------------------------------------------

Title: Blue Robotics Thruster Commander Firmware - Low-Pass Filter

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

#include "LPFilter.h"
#include "Thruster-Commander.h"

//////////////////
// Constructors //
//////////////////

// Default Constructor
LPFilter::LPFilter()
{
  _input  = PWM_NEUTRAL;
  _output = PWM_NEUTRAL;
}

// Destructor
LPFilter::~LPFilter() {} // Nothing to destruct


////////////////////
// Public Methods //
////////////////////

// Move filter along one timestep, return filtered output
float LPFilter::step(float input)
{
  // Update input
  _input = input;

  // Initialize output value
  float output = 0;

  // Run filter
  // Handle input
  output += FILTER_DT/FILTER_TAU*_input;

  // Handle output
  output -= (FILTER_DT/FILTER_TAU - 1)*_output;

  // Save latest output
  _output = output;

  return output;
}

