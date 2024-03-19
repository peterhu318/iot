#define I2CBUS_0	"/dev/i2c-0"
#define I2CBUS_1	"/dev/i2c-1"

float yl();		//压力传感器	
float ll();		//流量传感器

//全局变量
/*
char*	MQTTSERVER;

char*	MQTTPORT;
char*   
*/

#define MAX_LINELEN	200
char ADDRESS[MAX_LINELEN];
char CLIENTID[MAX_LINELEN];
char USERNAME[MAX_LINELEN];
char PASSWORD[MAX_LINELEN];
char TOPIC[MAX_LINELEN];
char SUBTOPIC[MAX_LINELEN];

char RUN_MODE='f';
char JSON_STR[MAX_LINELEN];
