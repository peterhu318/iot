#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>


#define SLAVE_ADDR  0x6d
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

  /* read 0xa5 */
        data[0] = 0xa5;
        if (write(i2c_bus,data,1) != 1) {
                perror("error set reg address");
                return -3;
        }
        if(read(i2c_bus,data, 1) !=1 ) {
                perror("error read temp value");
                return -4;
        }
        printf("Read: %hx  %hx\n",data[0],data[1]);
        
  /* write 0xa5 */
        data[1] = data[0]&0b11111101;
        data[0] = 0xa5;
      
        if (write(i2c_bus,data,2) != 2) {
                perror("error set reg address");
                return -3;
        }

        printf("Final: %hx  %hx\n",data[0],data[1]);

  /* read 0xa5 */
        data[0] = 0xa5;
        if (write(i2c_bus,data,1) != 1) {
                perror("error set reg address");
                return -3;
        }
        if(read(i2c_bus,data, 1) !=1 ) {
                perror("error read temp value");
                return -4;
        }
        printf("Read2: %hx  %hx\n",data[0],data[1]);

        close(i2c_bus);
        return 0;
}

        

