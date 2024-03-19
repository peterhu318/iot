#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
/*
1、读0xa5, 将数值与1111 1101进行与运算，然后再写回0xa5
2、设置0x30寄存器值为0xa0
3、读0x6,7,8三个字节，此为压力值。其中，6为高位，8为低位
4、如果数值为正，压力值为所得数值/n, n的取值根据量程来算。具体查看手册
*/


#define SLAVE_ADDR  0x6d

int main(int argc, char *argv[]) {
        int i2c_bus;
        uint8_t data[2];
        uint8_t press[3];

        /* Open the bus */
        i2c_bus = open("/dev/i2c-0",O_RDWR);
        if (i2c_bus < 0) {
           perror("Error opening i2c bus");
           return -1;
        }
        
        /* Open slave device */
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
        printf("Before Read: %hx\n",data[0]);
        
  /* write 0xa5 */
        data[1] = data[0]&0b11111101;
        data[0] = 0xa5;
      
        if (write(i2c_bus,data,2) != 2) {
                perror("error set reg address");
                return -3;
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
        printf("Afeter Read: %hx\n",data[0]);

/* 0x30 写入值0x0a(一次温度采集，一次压力采集) */
        data[1] = 0x0a;
        data[0] = 0x30;
      
        if (write(i2c_bus,data,2) != 2) {
                perror("error set reg address");
                return -3;
        }

/* 0x30 读取 */
        data[1] = 0x0a;
        data[0] = 0x30;
      
        if (write(i2c_bus,data,1) != 1) {
                perror("error set reg address");
                return -3;
        }
        if(read(i2c_bus,data, 1) !=1 ) {
                perror("error read temp value");
                return -4;
        }
        printf("0x30 write 0x0a,Read: %hx  \n",data[0]);


/* 读取 678的压力数*/
        sleep(1);    //等待1s
        press[0] = 0x06;
        press[1] = 0x00;
        press[2] = 0x00;
        if (write(i2c_bus,press,1) != 1) {
                perror("error set reg address");
                return -3;
        }
        if(read(i2c_bus,press, 3) !=3 ) {
                perror("error read temp value");
                return -4;
        }
        printf("0x30press: %hx  %hx  %hx\n",press[0],press[1],press[2]);
        //printf("0x30press: %hx  %hx  %hx\n",press[0] 
/* 解析press数组 */
        uint32_t rtn;
        uint32_t rtn_tmp,t1,t2,t3;
        t1=(int)press[0]*256*256;
        t2=(int)press[1]*256;
        t3=(int)press[2];
        printf("three t value: %d %d %d\n",t1,t2,t3);
        
        t1=(uint32_t)press[0]<<16;
        t2=(uint32_t)press[1]<<8;
        t3=(uint32_t)press[2];
        printf("three t value: %u %u %u\n",t1,t2,t3);
        rtn_tmp=t1+t2+t3;
        if ( (int)press[0]<128 ) {
            rtn=((uint32_t)press[0]<<16) + ((uint32_t)press[1]<<8) + (uint32_t)press[2];
        } else {
                rtn=111;
        }
        
        
        printf("Cnreturn value:%u\n",rtn/32);
        printf("Wyreturn value:%u\n",rtn_tmp);
        
        close(i2c_bus);
        return 0;
}

        

