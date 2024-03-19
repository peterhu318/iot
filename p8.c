#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>


#define SLAVE_ADDR  0x50
#define u16	    uint16_t

int main(int argc, char *argv[]) {
	int i2c_bus;
	uint8_t data[5];

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
	data[2] = 0x00;
	data[3] = 0x00;
	data[4] = 0x00;
	if(read(i2c_bus,data, 5) !=5 ) {
		perror("error read temp value");
		return -4;
	}
	printf("%x %x %x %x %x \n",data[0],data[1],data[2],data[3],data[4]);

	uint8_t checksum;
        float result;
	checksum = data[1]+ data[2]+ data[3]+ data[4];
	checksum = ~checksum;
	checksum += 1;
	printf("%x\n",checksum);
	if(checksum == data[0])
	{
		uint16_t tmp;
		tmp = (u16) data[1]<<8;
	        tmp = tmp	+ data[2];
		result = (tmp-6553)*50/52428; 
		printf("count=%d clibrate:%.2f\n",tmp,result);
	}
	else
	{
		
		printf("checksum error\n");
	}

	close(i2c_bus);

	return 0;
}

