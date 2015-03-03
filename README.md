# si4703
This folder contains the various files that I'm working on for the Si4703 breakout board. (These are functional.)  For instructions on compiling, read the source code for each.

- **registers.c** - Has no control over the chip but dumps the Si4703's registers to the command line.  (Note: you need to run si4703-init first, or registers.c will dump mostly 0's.  Requires no argument.
- **reset-bus.sh** - A short script that resets the GPIO pin config.  This was valuable while working out the code.  Requires no argument.
- **si4703-init.c** - Initializes the Si4703 and powers it on.  This command needs to be run before any other control.  Note: this is the only remaining binary in this set that requires the wiringPi library.  If you don't run this command, "i2cdetect -y 1" will not "see" the chip on the I2C bus.  Requires no argument. (Update: Now have a working init binary that doesn't require wiringPi.  Will add in next update. - 2 Mar 2015)
- **status.c** - A simplified version of registers.c.  Shows: the current FM radio station, the current volume, the current signal strength, and if stereo was detected.
- **tune.c** - Tunes the chip to a specific frequency.  Requires one argument: frequency in MHz (e.g., "./tune 98.7").
- **volume.c** - Sets the volume of the chip's output.  Requires one argument: a value between 1 and 15.

Known issues:
- (1 Mar 2015) For some reason, listening to certain RDS-enabled stations can trigger a reset of the chip.  This may be a function of my hardware (RPi, breadboard, power supply, my wife's cat, etc.).  Will provide updates as I better determine possible causes.
- (1 Mar 2015) status.c checks the incorrect register for the currently tuned channel.  Register 0x03 contains the last frequency entered by the user and this value doesn't change when the seek function is employed.  The proper register to read is 0x0B.  I will correct this in the next update.
- (1 Mar 2015) si4703-init.c requires wiringPi library.  This is a bit overkill to just toggle a GPIO pin to reset the Si4703.  Update (2 Mar): now have a working binary that doesn't require the wiringPi library.  Will upload code in the next update.
