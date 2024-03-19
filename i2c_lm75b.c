#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>


#define SLAVE_ADDR  0x48
#define TEMP_ADDR   0x00

int main(int argc, char *argv[]) {
	int i2c_bus;
	uint8_t data[2];
	/* Open the bus */
	i2c_bus = open("/dev/i2c-0",O_RDWR);
	if (i2c_bus < 0) {
	   perror("Error opening i2c bus");
	   return -1;
	}
	
	if (ioctl(i2c_bus,I2C_SLAVE,SLAVE_ADDR) < 0) {
	   perror("Error opening i2c slave");
	   return -2; 
	}
	
	/* begin read temp */
	data[0] = 0x00;
	data[1] = 0x00;
	if (write(i2c_bus,data,1) != 1) {

		perror("error set reg address");
		return -3;
	}

	if(read(i2c_bus,data, 2) !=2 ) {
		perror("error read temp value");
		return -4;
	}
	if (data[0] < 128) {

  	printf("%hx  %hx\n",data[0],data[1]);	
  	float t;
	int16_t i16=(data[0]<<8) + data[1];
        i16=i16>>5;
        t=i16*0.125;	
	printf("%.2f",t);
	}
	close(i2c_bus);

	return 0;
}

