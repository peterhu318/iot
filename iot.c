#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <MQTTClient.h>
#include "iot.h"

//后台运行所需头文件
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <fcntl.h>

#define QOS         1
#define TIMEOUT     10000L

// for rand include,can delete it
#include <time.h>

//1、订阅消息处理函数
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
volatile MQTTClient_deliveryToken deliveredtoken;
void delivered(void *context, MQTTClient_deliveryToken dt)
{
    printf("Message with token value %d delivery confirmed\n", dt);
    deliveredtoken = dt;
}

int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{
    printf("Message arrived\n");
    printf("     topic: %s\n", topicName);
    printf("     message: %.*s\n", message->payloadlen, (char*)message->payload);
    //测试
    if (strcmp((char*)message->payload,"aa") == 0 ) {
	    printf("收到退出命令\n");
	    exit(0);
    }

    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1;
}

void connlost(void *context, char *cause)
{
    printf("\nConnection lost\n");
    printf("     cause: %s\n", cause);
}

//2、配置文件解析函数
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void readConfigFile(const char *filename) {
    //部分变量进行赋初始值
    strcpy(USERNAME,"username");
    strcpy(PASSWORD,"password");

    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Error opening file %s\n", filename);
        exit(1);
    }
    char line[MAX_LINELEN];
    char key[MAX_LINELEN];
    char value[MAX_LINELEN];

    while (fgets(line, MAX_LINELEN, file) != NULL) {
	if (line[0]=='#') {
	    continue;
	}
        if (sscanf(line, "%s %s", key, value) == 2) {
            if (strcmp(key, "ADDRESS") == 0) {
                strcpy(ADDRESS,value);
            } else if (strcmp(key, "CLIENTID") == 0) {
                strcpy(CLIENTID,value);
            } else if (strcmp(key, "USERNAME") == 0) {
                strcpy(USERNAME,value);
            } else if (strcmp(key, "PASSWORD") == 0) {
                strcpy(PASSWORD,value);
            } else if (strcmp(key, "TOPIC") == 0) {
                strcpy(TOPIC,value);
            } else if (strcmp(key, "SUBTOPIC") == 0) {
                strcpy(SUBTOPIC,value);

            } else if (strcmp(key, "RUN_MODE") == 0) {
                RUN_MODE=value[0];
            } else if (strcmp(key, "JSON_STR") == 0) {
                strcpy(JSON_STR,value);
	    }
        }
    }
    fclose(file);
}

//读取命令行参数
void parse_para(int argc, char** argv) {
	int o;
	while ((o = getopt(argc, argv, "bf")) != -1) {
	  switch (o) {
	    case 'b':
	       RUN_MODE='b';
	       break;
	    case 'f':
	       RUN_MODE='f';
	       break;
	    case '?':
	       printf("\nusage: iot options\n"
                      "       -b  back running\n"
                      "       -f  front running\n");
	       exit(-1);
	   }  
    }
}

int main(int argc, char* argv[]) {
    MQTTClient client;
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    MQTTClient_deliveryToken token;
    int rc;
    char payload[500];

    //读取配置文件
    printf("读取配置文件中......");
    readConfigFile("iot.cfg");
    printf("读取完成！\n");    
    // 读取命令行参数，优先级高于配置文件
    parse_para(argc,argv);

    //进入子进程，以后台方式运行
    int  pid_t, pid;
    if (RUN_MODE=='b') {
        // 创建子进程
        pid = fork();
        if (pid < 0) {
            printf("Fork failed\n");
            exit(1);
        }
        // 如果是父进程，退出程序
        if (pid > 0) {
            printf("父进程退出!\n");
            exit(0);
        }
        // 在子进程中，创建新会话，并成为新会话的首进程
        setsid();
        // 修改当前目录为根目录，以防止占用被挂载的文件系统
        chdir("/");
        // 设置文件权限掩码，防止创建的文件受到继承的影响
        umask(0);
        // 关闭标准输入、标准输出和标准错误输出
        printf("后台进程成功运行.....\n");
        close(STDIN_FILENO);
        fclose(stdout);
        close(STDERR_FILENO);
    }

    conn_opts.username = USERNAME;
    conn_opts.password = PASSWORD;
    MQTTClient_create(&client, ADDRESS, CLIENTID,
        MQTTCLIENT_PERSISTENCE_NONE, NULL);

    if ((rc = MQTTClient_setCallbacks(client, NULL, connlost, msgarrvd, delivered)) != MQTTCLIENT_SUCCESS)
    {
        printf("Failed to set callbacks, return code %d\n", rc);
        rc = EXIT_FAILURE;
        goto destroy_exit;
    }

    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;
    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS) {
        printf("Failed to connect, return code %d\n", rc);
        return -1;
    }

    if ((rc = MQTTClient_subscribe(client, SUBTOPIC, QOS)) != MQTTCLIENT_SUCCESS)
    {
    	printf("Failed to subscribe, return code %d\n", rc);
    	rc = EXIT_FAILURE;
		goto destroy_exit;
    }

srand((unsigned)time(NULL));
    pubmsg.qos = QOS;
    pubmsg.retained = 0;
    while (1) {
        float sensor1_data = 25.51;
        float sensor2_data = 30.02;
        float sensor3_data = 25.51;
        float sensor4_data = 30.02;
    	sensor1_data = ll(I2CBUS_0,0x50);
	sensor2_data = yl(I2CBUS_0,0x6d);
	sensor3_data = ll(I2CBUS_1,0x50);
	sensor4_data = yl(I2CBUS_1,0x6d);

sensor1_data+=rand()%30;
sensor2_data+=rand()%40;
sensor3_data+=rand()%50;
sensor4_data+=rand()%60;

        //sprintf(payload, "{\"sensor1\": %.2f, \"sensor2\": %.2f}", sensor1_data, sensor2_data,sensor3_data,sensor4_data);
        sprintf(payload, JSON_STR, sensor1_data, sensor2_data,sensor3_data,sensor4_data);
        pubmsg.payload = payload;
        pubmsg.payloadlen = strlen(payload);
        MQTTClient_publishMessage(client, TOPIC, &pubmsg, &token);
        printf("Waiting %d seconds, publication of %s\n"
               "topic %s  ClientID: %s\n",
               (int)(TIMEOUT/1000), payload, TOPIC, CLIENTID);
        rc = MQTTClient_waitForCompletion(client, token, TIMEOUT);
//        printf("Message with delivery token %d delivered\n", token);
	//sleep(3);
    }

destroy_exit:
    MQTTClient_destroy(&client);
exit:
    return rc;
}

