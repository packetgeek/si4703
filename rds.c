
#include <stdio.h> 
#include <stdlib.h> 
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <inttypes.h>
#include <wiringPi.h>
#include <string.h>

uint16_t buf[16];
uint16_t reg[16];

uint8_t Ah, Al, Bh, Bl, Ch, Cl, Dh, Dl;

typedef uint8_t boolean;
typedef uint8_t byte;

#define true 1
#define false 0

// compile via "gcc -o status status.c -lwiringPi"

// following opens access to the FM receiver
int open_chip() {
	int fd;
	int addr = 0x10;
	char *dev="/dev/i2c-1";
	
	if((fd=open(dev, O_RDWR))<0) {
		printf("error opening i2c device node\n");
		exit(1);
	}
	if(ioctl(fd, I2C_SLAVE, addr) <0) {
		printf("error selecting i2c device\n");
		close(fd);
		exit(1);
	}
	return fd;
}

// following simply closes access to the receiver
void close_chip(int fd) {
	close(fd);
}

// it's bad form but following reads the FM receiver's
// registers into global variables (the "bad form" being
// use of global variables)(will fix later)

void read_registers(int fd) {
	int a;

	read(fd,buf,32);

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
	
	//for(a=0;a<16;a++){
	//	printf("0X%02X:\t%04X\n",a,reg[a]);
	//}
	return;
}

// spec sheet says frequency is in register 03h
float get_frequency() {
	//int channel=(reg[0x03] & 0x1FF);
	int channel=(reg[0x0B] & 0x1FF);
	float frequency=.1*channel+87.5;
	return frequency;
}

// spec sheet says stereo indicate is bit 8 in register 0Ah
int detect_stereo() {
	int fd = open_chip();
	read_registers(fd);
	close(fd);

	int stereo = (reg[0x0A] & 0x0100)>>8;
	//printf("stereo = %x\n",stereo);
	return stereo;
}

int get_signal_strength() {
	int fd = open_chip();
	read_registers(fd);
	close(fd);

	int signal = (reg[0x0A] & 0xFF);
	//printf("0x0A = %04X\n",reg[0x0A]);
	return signal;
}

int get_volume() {
	int fd = open_chip();
	read_registers(fd);
	close(fd);

	int volume = (reg[0x05] & 0xF);
	return volume;
}

int test_rds() {
	int fd = open_chip();
	read_registers(fd);
	close(fd);

	int rdsdet=((reg[0x0A] & (1<<15))>>15);
	return rdsdet;
}

void readRDS(int fd, char* buffer, long timeout) {
	long endTime = millis() + timeout;
	boolean completed[] = {false, false, false, false};
	int completedCount=0;

	while (completedCount<4 && millis()<endTime) {
		read_registers(fd);
		if(reg[0x0A] & (1<<15)) {
			uint16_t b = reg[0x0D];
			int index = b & 0x03;

			if (!completed[index] && b < 500) {
				completed[index] = true;
				completedCount++;
				char Dh = (reg[0x0F] & 0xFF00)>>8;
				char Dl = (reg[0x0F] & 0x00FF);
				buffer[index *2] = Dh;
				buffer[index *2+1]=Dl;
			}
			delay(50); //originally was set to 40
		} else {
			delay(30);
		}
	}
	if (millis() >= endTime) {
		buffer[0]='\0';
		return;
	}
	buffer[8]='\0';
}

int main( int argc, char *argv[]) {
	int fd;
	fd=open_chip();
	read_registers(fd);
	close_chip(fd);
	
	printf("freq\t= %0.1F\n",get_frequency());

	printf("stereo\t= ");
	if(detect_stereo()==0) {
		printf("no\n");
	} else {
		printf("yes\n");
	}
	printf("signal\t= %02d\n",(int)get_signal_strength());
	printf("volume\t= %02d\n",(int)get_volume());
	printf("rds\t= %d\n",test_rds());
	
	fd=open_chip();
	char oldbuffer[10]={0};
	while(1) {
		char buffer[10]={0};
		readRDS(fd,buffer,15000);
		if(strncmp(oldbuffer,buffer,10)==0) {
		} else {
			printf("%s\n",buffer);
		}
		strncpy(oldbuffer,buffer,10);
	}
	//read_registers(fd);
	close(fd);
	return;
}
