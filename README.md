# si4703
This folder contains the various files that I'm working on for the Si4703 breakout board.  For instructions on compiling, read the source code for each.

* registers.c - Has no control over the chip but dumps the Si4703's registers to the command line.  (Note: you need to run si4703-init first, or registers.c will dump mostly 0's.  Requires no argument.
* reset-bus.sh - A short script that resets the GPIO pin config.  This was valuable while working out the code.  Requires no argument.
* si4703-init.c - Initializes the Si4703 and powers it on.  This command needs to be run before any other control.  Note: this is the only remaining binary in this set that requires the wiringPi library.  If you don't run this command, "i2cdetect -y 1" will not "see" the chip on the I2C bus.  Requires no argument.
* status.c - A simplified version of registers.c.  Shows: the current FM radio station, the current volume, the current signal strength, and if stereo was detected.
* tune.c - Tunes the chip to a specific frequency.  Requires one argument: frequency in MHz (e.g., "./tune 98.7").
* volume.c - Sets the volume of the chip's output.  Requires one argument: a value between 1 and 15.

Known issues:
* (1 Mar 2015) For some reason, listening to certain RDS-enabled stations can trigger a reset of the chip.  This may be a function of my hardware (RPi, breadboard, power supply, my wife's cat, etc.).  Will provide updates as I better determine possible causes.
