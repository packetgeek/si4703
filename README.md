# si4703
This folder contains the various files that I'm working on for the Si4703 breakout board. (These are functional.)  For instructions on compiling, read the source code for each.

- **rds.c** - Very cheesy code to show the RDS data.  If the output looks a bit weird, try adjusting the timeout.
- **registers.c** - Has no control over the chip but dumps the Si4703's registers to the command line.  (Note: you need to run si4703-init first, or registers.c will dump mostly 0's.  Requires no argument.
- **reset-bus.sh** - A short script that resets the GPIO pin config.  This was valuable while working out the code.  Requires no argument.
- **seekdn.c** - Tunes downward to the next radio station.  Minimal sensitivity hard code (edit to your taste).  Requires no argument.
- **seekup.c** - Tunes upward to the next radio station.  Minimal sensitivity hard code (edit to your taste).  Requires no argument.
- **si4703-init.c** - Initializes the Si4703, powers it on, and sets initial volume to 5 (out of 15).  This command needs to be run before any other control.  Recommend setting the sticky bit on this one, to avoid using "sudo".  Note: this version does not require the wiringPi library.
- **status.c** - A simplified version of registers.c.  Shows: the current FM radio station, the current volume, the current signal strength, and if stereo was detected.
- **tune.c** - Tunes the chip to a specific frequency.  Requires one argument: frequency in MHz (e.g., "./tune 98.7").
- **volume.c** - Sets the volume of the chip's output.  Requires one argument: a value between 1 and 15.

Known issues:
- (15 Mar 2015) rds.c still requires the wiringPi library (for the timeout functions).  Will need to rewrite this in the future.
- (1 Mar 2016) Certain portions of the code are not needed due to the normally available Si4703 breakout boards being hardwired.  The code doesn't appear to affect anything, it's just non-functional because of the "hardwiring".  Short version: it's been pointed out that this code could be shorter.  I'll get around to editing it eventually.

