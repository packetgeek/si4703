
#include <stdio.h> 
#include <stdlib.h> 
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <inttypes.h>
#include <unistd.h>

// compile with gcc -o tune tune.c

uint16_t buf[16];
uint16_t reg[16];

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
	int channel=(reg[0x03] & 0x1FF);
	float frequency=.1*channel+87.5;
	return frequency;
}

// spec sheet says stereo indicate is bit 8 in register 0Ah
int detect_stereo() {
	int stereo=(reg[0x0A] & 0x100)>>8;
	return stereo;
}

void write_registers() {
	int dID = 0x10;
	int fd;

	fd=open_chip();

	int i;
	int r=0;
	uint16_t newreg[6];

	//convert back to big endian
	for(i=0x02;i<0x08;i++) {
		newreg[r]=(reg[i]>>8)|(reg[i]<<8);
		r++;
	}
//	for(i=0;i<r;i++) {
//		printf("%d %04X\n",i,newreg[i]);
//	}
	if(write(fd,newreg,12) < 12) {
		printf("could not write to device\n");
		exit(1);
	}
	close(fd);
}

int main( int argc, char *argv[]) {
	int fd;
	fd=open_chip();
	read_registers(fd);
	close_chip(fd);
	
	float freq= strtod(argv[1],NULL);
	//printf("f = %.1f\n",freq);
	int channel=(freq*10)-875;
	//printf("c = %d or %x\n",channel,channel);

	reg[3] &= 0xFE00; //set channel to 00
	reg[3] |= channel; // set the new channel
	reg[3] |= (1<<15); // set the tune bit

	write_registers();

	usleep(80000);

	while(1) {
		fd=open_chip();
		read_registers(fd);
		close_chip(fd);
		if((reg[0x0A] & (1<<14)) !=0) break;
	}

	fd=open_chip();
	read_registers(fd);
	close_chip(fd);

	reg[3] &= ~(1<<15); // clear the tune bit

	write_registers();

	//wait for the radio to clear the STC bit

	while(1) {
		fd=open_chip();
		read_registers(fd);
		close_chip(fd);
		if((reg[0x0A] & (1<<14)) == 0) break;
	}

}
