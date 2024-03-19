#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <fcntl.h>
#include <unistd.h>

int main() {
pid_t pid;

// 创建子进程
pid = fork();

// 如果创建失败，则退出程序
if (pid < 0) {
printf("Fork failed\n");
exit(1);
}

// 如果是父进程，退出程序
if (pid > 0) {
printf("I am Master,exit");
exit(0);
} 
else {
printf("I am Sub...ing...%d\n",pid);
}

// 在子进程中，创建新会话，并成为新会话的首进程
setsid();

// 修改当前目录为根目录，以防止占用被挂载的文件系统
chdir("/");

// 设置文件权限掩码，防止创建的文件受到继承的影响
umask(0);

// 关闭标准输入、标准输出和标准错误输出
close(STDIN_FILENO);
close(STDOUT_FILENO);
close(STDERR_FILENO);

// 执行后台任务，这里可以写你的业务逻辑
// 这里只是一个示例，打印一条信息并睡眠 10 秒钟
while (1) {
printf("Running in the background...\n");
sleep(10);
}

return 0;
}

