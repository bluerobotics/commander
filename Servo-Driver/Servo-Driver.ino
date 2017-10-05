//#include <avr/io.h>
//#include <avr/interrupt.h>

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

bool ext0IsConnected, ext1IsConnected;
int pwmOut0, pwmOut1;

void setup() {
  // Set up pin modes
  pinMode(INPUT_INT,INPUT);
  pinMode(INPUT_EXT0,INPUT);
  pinMode(INPUT_EXT1,INPUT);
  pinMode(LED_INT,OUTPUT);
  pinMode(LED_EXT0,OUTPUT);
  pinMode(LED_EXT1,OUTPUT);
  pinMode(PWM_0,OUTPUT);
  pinMode(PWM_1,OUTPUT);

  digitalWrite(LED_INT,HIGH);
  delay(200);
  digitalWrite(LED_INT,LOW);
  delay(200);

  // Set PWM output (temporary)
  pwmOut0 = 1500;
  pwmOut1 = 1500;

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
  
  // Set timer1 Output Compare Register A
  // Set shut-off counter value to get pwmOut0 pulse
  OCR1A   = (pwmOut0 * CNT_PER_US) - 1;

  // Set timer1 Output Compare Register B
  // Set shut-off counter value to get pwmOut pulse
  OCR1B   = (pwmOut1  * CNT_PER_US) - 1;
  
  // Done setting timers -> allow interrupts again
  sei();
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(LED_INT,HIGH);
  delay(200);

  digitalWrite(LED_INT,LOW);
  delay(800);
}
