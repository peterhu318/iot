#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

#define PCA9548A_ADDR	0x70
#define I2C_BUS		"/dev/i2c-3"

/* 通道选择: 0~7 */
int pca9548a_select(int channel) {
	int i2c_bus;
	uint8_t data[1];

	/* Open the bus */
	i2c_bus = open(I2C_BUS,O_RDWR);
	if (i2c_bus < 0) {
	   perror("Error opening i2c bus");
	   return -1;
	}

	if (ioctl(i2c_bus,I2C_SLAVE,PCA9548A_ADDR) < 0) {
	   perror("Error opening i2c slave");
	   return -2; 
	}

	/* set channel */
	if (channel>7 || channel <0) 
		data[0]=0x00;
	else
		data[0] = (1 <<	(channel&0xff));

	if (write(i2c_bus,data,1) != 1) {
                perror("error set channel address");
                return -3;
    	}

	/* get channel */
	if (read(i2c_bus,data,1) != 1) {
                perror("error get channel address");
                return -3;
    	}
	close(i2c_bus);
	return (int)data[0];
}

int main(int argn, char* argv[]) {
	int channel;
	int val;
	printf("Input a int number(0-7):");
	scanf("%d",&channel);
	val=pca9548a_select(channel);
	printf("return val is %d",val);
	return 0;
}

