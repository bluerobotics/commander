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
#define HALF_RANGE  500               // us
#define DEADZONE    25                // us

struct blinker_t {
  unsigned long lastblink;  // timestamp (ms)
  unsigned int  period;     // ms
  int           pin;
  bool          state;
};

bool      ext0IsConnected, ext1IsConnected;

int       pwmOut0, pwmOut1;
blinker_t leds[3];    // 0:ext0, 1:ext1, 2:int

void setup() {
  // Set up pin modes
  pinMode(INPUT_INT,INPUT);
  pinMode(INPUT_EXT0,INPUT);
  pinMode(INPUT_EXT1,INPUT);
  pinMode(DETECT,OUTPUT);
  pinMode(LED_INT,OUTPUT);
  pinMode(LED_EXT0,OUTPUT);
  pinMode(LED_EXT1,OUTPUT);
  pinMode(PWM_0,OUTPUT);
  pinMode(PWM_1,OUTPUT);

  // Initialize motor controllers
  initializePWMController();
  writePWM0(1500);
  writePWM1(1500);

  // Initialize all 3 LEDs
  initializeLEDs();

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
      pwmOut0 = constrain(pwmBase+pwmSteer,PWM_MIN,PWM_MAX);;
      pwmOut1 = constrain(pwmBase-pwmSteer,PWM_MIN,PWM_MAX);;
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

  // Set pwm outputs
  writePWM0(pwmOut0);
  writePWM1(pwmOut1);

  setLEDs();

  delay(50); // About 20 Hz update rate
}

void writePWM0(int pulsewidth) {
  // Stop interrupts while changing pwm settings
  cli();

  // Constrain pulsewidth
  pulsewidth = constrain(pulsewidth,PWM_MIN,PWM_MAX);

  // Set timer1 Output Compare Register A
  // Set shut-off counter value to get pulsewidth us pulse
  OCR1A = (pulsewidth * CNT_PER_US) - 1;

  // Done setting timers -> allow interrupts again
  sei();
}

void writePWM1(int pulsewidth) {
  // Stop interrupts while changing pwm settings
  cli();

  // Constrain pulsewidth
  pulsewidth = constrain(pulsewidth,PWM_MIN,PWM_MAX);

  // Set timer1 Output Compare Register B
  // Set shut-off counter value to get pulsewidth us pulse
  OCR1B = (pulsewidth * CNT_PER_US) - 1;

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

void initializeLEDs() {
  leds[0].pin = LED_EXT0;
  leds[1].pin = LED_EXT1;
  leds[2].pin = LED_INT;

  for ( int i = 0; i < 3; i++) {
    leds[i].lastblink = 0;
    leds[i].period    = 0;
    leds[i].state     = 0;
  }
}

void setLEDs() {
  // Update LED periods
  leds[0].period = pwmToBlinkPeriod(pwmOut0); // ext0
  leds[1].period = pwmToBlinkPeriod(pwmOut1); // ext1
//  leds[2].period = 1000;                      // int

  // Blink each LED if necessary
  for(int i=0; i<3; i++) {
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

