/* Blue Robotics Thruster Commander Firmware
-----------------------------------------------------

Title: Blue Robotics Thruster Commander Firmware - Acceleration Limiter

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

#include "Limiter.h"

// DEFAULT VALUES
#define DEFAULT_MAX_ACCEL 50      // us/s

//////////////////
// Constructors //
//////////////////

// Default Constructor
Limiter::Limiter() {
  this->_lastoutput   = 0;
  this->_maxaccel     = DEFAULT_MAX_ACCEL;
  this->_lastruntime  = millis();
}

// Useful Constructor
Limiter::Limiter(float maxaccel, float startvalue) {
  this->_lastoutput   = startvalue;
  this->_maxaccel     = maxaccel;
  this->_lastruntime  = millis();
}

// Destructor
Limiter::~Limiter() {} // Nothing to destruct


////////////////////
// Public Methods //
////////////////////

// Move filter along one timestep, return filtered output
float Limiter::step(float input) {
  // Measure elapsed time
  float dt  = (millis() - this->_lastruntime)/1000.0f;
  this->_lastruntime = millis();

  // Calculate maximum/minimum allowable values this round
  float max = this->_lastoutput + dt*(this->_maxaccel);
  float min = this->_lastoutput - dt*(this->_maxaccel);

  // Limit output value
  float output = (input > max) ? max : ((input < min) ? min : input);

  // Save latest output
  this->_lastoutput = output;

  return output;
}
