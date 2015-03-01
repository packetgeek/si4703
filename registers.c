#include <stdio.h> 
#include <stdlib.h> 
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <inttypes.h>

int fd,r,mine;

char *dev="/dev/i2c-1";
int addr = 0x10;
//int dID = 0x10;

uint16_t buf[16];
uint16_t reg[16];

char *desc[16];

void read_registers() {
	int a;

	fd=open(dev, O_RDWR);
	if(fd <0){
		perror("opening i2c device node\n");
	}
	r=ioctl(fd, I2C_SLAVE, addr);
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
	
	desc[0]="device id";
	desc[1]="chip id";
	desc[2]="power config (mute, seek, rdsmode)";
	desc[3]="channel (tune)";
	desc[4]="system config 1";
	desc[5]="system config 2 (volume, band select, spacing)";
	desc[6]="system config 3";
	desc[7]="test 1";
	desc[8]="test 2";
	desc[9]="boot config";
	desc[10]="status rssi (stereo ind, signal strength)";
	desc[11]="read channel";
	desc[12]="RDSA";
	desc[13]="RDSB";
	desc[14]="RDSC";
	desc[15]="RDSD";

	for(a=0;a<16;a++){
		printf("0X%02X:\t%04X  <-- %s\n",a,reg[a],desc[a]);
	}
}

int main( int argc, char *argv[]) {

	read_registers();
	return(0);
}
