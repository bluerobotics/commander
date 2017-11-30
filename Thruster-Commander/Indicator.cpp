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

#include "Indicator.h"
#include "Thruster-Commander.h"

namespace {
  uint16_t  pattern;
  uint8_t   patternindex;
  bool      blinking;
}

///////////////
// Functions //
///////////////

// Initialize LEDs for blinking mode, set bits to enable Fast PWM mode
void initializeLEDs() {
  // Note: timer0 clock source defaults to prescaler 64 (will run at 488 Hz)
  // Do not change!  Doing so will mess up millis(), etc, as they use timer0 too

  // Set Fast PWM mode
  bitSet(TCCR0A, WGM00);
  bitSet(TCCR0A, WGM01);

  // Start initialized for blinking
  setBlinkerBits();
  blinking = true;

  // Enable timer0 OCR0A compare interrupt (for blinking)
  bitSet(TIMSK0, OCIE0A);
}

// Set dimmer value
void writeDimmer(int pin, int pwm) {
  // Check whether we were previously in Blinker mode
  if ( blinking ) {
    // Stop blinking
    blinking = false;
    // Set up for dimming mode
    setDimmerBits();
  }

  // Stop interrupts while changing pwm settings
  cli();

  uint8_t period = constrain(map(abs(pwm - PWM_NEUTRAL), DEADZONE, HALF_RANGE,
                             255, 0), 0, 255);   // 255 is off (inverting mode)

  if ( pin == OC0A_PIN ) {
    // Set timer0 Output Compare Register A
    // Set shut-off counter value to get pulsewidth us pulse
    OCR0A = period;
  }
  if ( pin == OC0B_PIN ) {
    // Set timer0 Output Compare Register B
    // Set shut-off counter value to get pulsewidth us pulse
    OCR0B = period;
  }

  // Done setting timers -> allow interrupts again
  sei();
}

// Set blink pattern
void writeBlinker(uint16_t ptrn) {
  // Check whether we were previously in Blinker mode
  if ( !blinking ) {
    // Start blinking
    blinking = true;
    // Set up for dimming mode
    setBlinkerBits();
    // Reset pattern index
    patternindex = 0;
  }

  // If pattern is different, reset index, save new pattern
  if ( ptrn != pattern ) {
    // Reset pattern index
    patternindex = 0;
    // Save new pattern
    pattern = ptrn;
  }
}

// Switch to dimmer mode by setting register bits
void setDimmerBits() {
  // Stop interrupts while changing timer settings
  cli();

  // Set inverting Fast PWM mode on A and B
  bitSet(TCCR0A, COM0A0);
  bitSet(TCCR0A, COM0A1);
  bitSet(TCCR0A, COM0B0);
  bitSet(TCCR0A, COM0B1);

  // Done setting timers -> allow interrupts again
  sei();
}

// Switch to blinker mode by setting register bits
void setBlinkerBits() {
  // Stop interrupts while changing timer settings
  cli();

  // Disable inverting Fast PWM mode on A and B
  bitClear(TCCR0A, COM0A0);
  bitClear(TCCR0A, COM0A1);
  bitClear(TCCR0A, COM0B0);
  bitClear(TCCR0A, COM0B1);

  // Done setting timers -> allow interrupts again
  sei();
}

///////////////////////////////
// Interrupt Service Routine //
///////////////////////////////

namespace {
  uint16_t  isrcounter;
}

// Triggered every time OCR0A matches TIMER0 counter (488 Hz)
SIGNAL(TIM0_COMPA_vect) {
  isrcounter++;
  if ( isrcounter >= 2*(F_CPU/64/256)/16 ) {
    isrcounter = 0;
    if ( blinking ) {
      digitalWrite(OC0A_PIN, bitRead(pattern, patternindex));
      digitalWrite(OC0B_PIN, bitRead(pattern, patternindex));
      if (patternindex < 15 ) {
        patternindex++;
      } else {
        patternindex = 0;
      }
    }
  }
}
