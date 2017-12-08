#!/bin/bash

# Color definitions
BLACK='\033[;30m'
RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'
PURPLE='\033[0;35m'
YELLOW='\033[1;33m'
WHITE='\033[1;37m'
NC='\033[0m'


# Configuration
avrdude="/home/daniel/Applications/arduino-1.8.5/hardware/tools/avr/bin/avrdude"
programmer="usbtiny"
conffile="/etc/avrdude.conf"
#conffile="/home/daniel/Applications/arduino-1.8.5/hardware/tools/avr/etc/avrdude.conf"
microcontroller="t84"
hexfile="/home/daniel/dev/Commander/commander/Thruster-Commander/Thruster-Commander.ino.tiny14.hex"

# Flash Thruster Commander
echo "Starting Thruster Commander Flash Sequence..."

# Write firmware
#echo "Writing Firmware..."
#$avrdude -C $conffile -c $programmer -p $microcontroller -U flash:w:$hexfile 2>&1 | tee firmware.log

# Set fuses
#echo "Setting Fuses..."
#$avrdude -C $conffile -c $programmer -p $microcontroller -e -Uefuse:w:0xff:m -Uhfuse:w:0xdf:m -Ulfuse:w:0xfe:m 2>&1 | tee fuses.log

#Write program and set fuses
$avrdude -C$conffile -c$programmer -p$microcontroller -e -Uefuse:w:0xff:m -Uhfuse:w:0xdf:m -Ulfuse:w:0xfe:m -Uflash:w:$hexfile 2>&1 | tee programcommander.log

echo -e "${BLUE}=========RESULT=========="
echo -e "${GREEN}"
grep "bytes of flash verified" programcommander.log
grep "avrdude: safemode: Fuses OK (E:FF, H:DF, L:FE)" programcommander.log
echo -e "${RED}"
grep "mismatch" programcommander.log
echo -e "${NC}"
echo -e "${BLUE}========================="
echo -e "${NC}"

rm programcommander.log

echo -e "${GREEN}Done. Please check for proper verification.${NC}"

echo -e "${GREEN}Press any key to repeat or Ctrl-C to quit.${NC}"
read -n 1 -s

./programCommander.sh


