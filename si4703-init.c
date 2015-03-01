#include <wiringPi.h> 
#include <wiringPiI2C.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <inttypes.h>

//compile with gcc -lwiringPi -o tune tune.c

// NOTE: pin schema used by gpio util and the wiringpi library are different

// Many thanks to Mark Kleback for providing info on board differences
// Prior to finding his article, I'd been using the wrong pins (4 & 8)
// which was cauing all I2C addresses to be active.  The proper pins
// for my Model B are 4 & 17.  See his article at:
// http://makezine.com/projects/tutorial-raspberry-pi-gpio-pins-and-python/

char readreg[32]="";
int resetpin = 4;
int sdiopin = 0;
int fd;

uint16_t buf[16];
uint16_t reg[16];

void set_I2C_mode() {
	wiringPiSetup();

	pinMode(resetpin,OUTPUT);
	pinMode(sdiopin,OUTPUT);

	digitalWrite(sdiopin,LOW);
	//pullUpDnControl(sdiopin,PUD_DOWN);
	sleep(.1);
	digitalWrite(resetpin,LOW);
	sleep(.1);
	//pullUpDnControl(resetpin,PUD_DOWN);
	digitalWrite(resetpin,HIGH);
	//pullUpDnControl(resetpin,PUD_UP);
	sleep(.1);
	return;
}

void read_registers() {
	int a;
	char *dev="/dev/i2c-1";
	int addr = 0x10;

	int fd=open(dev, O_RDWR);
	if(fd <0){
		perror("opening i2c device node\n");
	}
	int r=ioctl(fd, I2C_SLAVE, addr);
	if(r < 0) {
		perror("selecting i2c device\n");
	}

	read(fd,buf,32);
	close(fd);

	//reorg the values to match the spec sheet
	//also swap the first 8 bits with the last 8 bits

	reg[0]=(buf[6]>>8)|(buf[6]<<8);
	reg[1]=(buf[7]>>8)|(buf[7]<<8);
	reg[2]=(buf[8]>>8)|(buf[8]<<8);
	reg[3]=(buf[9]>>8)|(buf[9]<<8);
	reg[4]=(buf[10]>>8)|(buf[10]<<8);
	reg[5]=(buf[11]>>8)|(buf[11]<<8);
	reg[6]=(buf[12]>>8)|(buf[12]<<8);
	reg[7]=(buf[13]>>8)|(buf[13]<<8);
	reg[8]=(buf[14]>>8)|(buf[14]<<8);
	reg[9]=(buf[15]>>8)|(buf[15]<<8);
	reg[10]=(buf[0]>>8)|(buf[0]<<8);
	reg[11]=(buf[1]>>8)|(buf[1]<<8);
	reg[12]=(buf[2]>>8)|(buf[2]<<8);
	reg[13]=(buf[3]>>8)|(buf[3]<<8);
	reg[14]=(buf[4]>>8)|(buf[4]<<8);
	reg[15]=(buf[5]>>8)|(buf[5]<<8);
	
	for(a=0;a<16;a++){
		printf("0X%02X:\t%04X\n",a,reg[a]);
	}
	return;
}

void write_registers() {
	int dID = 0x10;
	int fd;

	if((fd=wiringPiI2CSetup(dID))<0) {
		printf("error opening i2c channel for write\n");
		exit(1);
	}

	int i;
	int r=0;
	uint16_t newreg[6];

	//convert back to big endian
	for(i=0x02;i<0x08;i++) {
		newreg[r]=(reg[i]>>8)|(reg[i]<<8);
		r++;
	}
	for(i=0;i<r;i++) {
		printf("%d %04X\n",i,newreg[i]);
	}
	if(write(fd,newreg,12) < 12) {
		printf("could not write to device\n");
		exit(1);
	}
	close(fd);
}

void set_volume(int volume) {
	read_registers();
	if(volume < 0) volume = 0;
	if(volume > 15) volume = 15;
	reg[0x05] &= 0xFFF0; // clear volume bits
	reg[0x05] |= volume; // set new volume
	write_registers();
}

int main( int argc, char *argv[]) {

	set_I2C_mode();
	read_registers();
	reg[0x07] = 0x8100; // enable the oscillatore
	write_registers();
	delay(500);

	reg[0x02]=0x4001; //enable the radio
	reg[0x04] |= (1<<12); //enable RDS
	reg[0x04] |= (0<<11); //european config
	reg[0x05] |= (1<<4); // 100kHz spacing
	write_registers();
	delay(110);

	set_volume(5);
}
