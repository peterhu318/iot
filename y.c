#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

/*大压力传感器 */

#define SLAVE_ADDR  0x6d
#define u16	    uint16_t

int main(int argc, char *argv[]) {
	int i2c_bus;
	uint8_t apdata[3],arange[4],adot[1];
	int32_t pdata;
	float rangeL;
	float rangeH;
	uint8_t dot;
	float result;

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
	
	/* begin pdata */
	apdata[0] = 0x06;
	apdata[1] = 0x00;
	apdata[2] = 0x00;

        if (write(i2c_bus,apdata,1) != 1) {
                perror("error set reg address");
                return -3;
        }

	if(read(i2c_bus,apdata, 3) !=3 ) {
		perror("error read data value");
		return -4;
	}
	printf("pdata:%x %x %x \n",apdata[0],apdata[1],apdata[2]);

	/* range 4bytes*/
	arange[0]=0xc5;
	arange[1]=0x00;
	arange[2]=0x00;
	arange[3]=0x00;
    if (write(i2c_bus,arange,1) != 1) {
        perror("error set reg address");
        return -3;
    }

	if(read(i2c_bus,arange, 4) !=4 ) {
		perror("error read data value");
		return -4;
	}
	printf("arange:%x %x %x %x\n",arange[0],arange[1],arange[2],arange[3]);

	/* dot */
	adot[0]=0xcf;
    if (write(i2c_bus,adot,1) != 1) {
        perror("error set reg address");
		return -3;
    }

	if(read(i2c_bus,adot,1) !=1 ) {
		perror("error read data value");
		return -4;
	}
	printf("adot:%x \n",adot[0]);

	/*parse date*/
	printf("--------------------\n");
	pdata=(((uint32_t)(apdata[0])<<16) + ((uint32_t)(apdata[1])<<8) + ((uint32_t)(apdata[2])<<0))&0xFFFFFF;
	if (apdata[0] & 0x80) {
		printf("高位为1\n");
		pdata |=0xFF000000;
	}

	printf("pdata=%u\n",pdata); 
	dot=(adot[0]>>5)&0x07;
	printf("dot=%d\n",dot);

	rangeL=(float)((int16_t)(((uint16_t)arange[0]<<8)+arange[1])) / pow(10,dot);
	rangeH=(float)((int16_t)(((uint16_t)arange[2]<<8)+arange[3])) / pow(10,dot);
	result=(rangeH-rangeL)*((float)(pdata-8388608*0.1)/(8388608*0.8))+rangeL;

	printf("rangeL=%f rangeH=%f\n",rangeL,rangeH);
	printf("result=%f\n",result);
	
	close(i2c_bus);
	return 0;
}
