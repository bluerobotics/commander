# Commander

Blue Robotics Thruster Commander hardware and software: Thruster Test Jig Version.

Acceleration is limited more in this version than is usual.  Changing speed from stopped to full speed takes 2.0 seconds (as opposed to 0.8 seconds for the default firmware).

## Compiling and Uploading

The code can be compiled in the Arduino IDE version 1.0 and above. It requires the `attiny` board plugin by David Mellis, which can be installed with the Arduino Boards Manager. The following compile settings should be used:

- **Board:** ATtiny
- **Processor:** ATtiny84
- **Clock:** 8 Mhz (External)

- **Programmer:** USBTinyISP (or whatever you're using)
