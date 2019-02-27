#include<stdio.h>
#include<unistd.h>
#include<sys/socket.h>
#include<string.h>
#include<netdb.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>

#define PORT 8900
#define BUF_SIZE 2048

int len;
struct sockaddr_in client;
struct sockaddr_in server;
char send_buf[BUF_SIZE];
char recv_buf[BUF_SIZE];
char cmd[2048];
int listend;
int opt;
int sendnum;
int recvnum;
int port;
int ret;
int flag;
int flag_exit;
// int i = 0;
// max_connect = 5;
// memset(send_buf,0,2048);
// memset(recv_buf,0,2048);

int execute(char* command,char* buf);

// char *mypopen(char * command, char *buf){
// 	int fd[2];
// 	int result =  pipe(fd);
// 	int status;
// 	pid_t pid = fork();
// 	if (pid==0)
// 	{
// 		close(fd[0]);
// 		dup2(fd[1],fd[0]);
// 		execl("/bin/sh","/bin/sh","-c",command,NULL);
// 		// printf("%s\n", buf);
// 		// write(fd[1],"123",3);


// 	}
// 	if (pid>0)
// 	{
// 		close(fd[1]);
// 		waitpid(pid, &status,0);
// 		read(fd[0], buf, 2048);
// 		return buf;
// 	}
// }
void test(int connectd){
    while (1){
        if(0>(recvnum = recv(connectd,recv_buf,sizeof(recv_buf),0)))
        {
            perror("error in recving data...\n");
            close(connectd);
        }
        recv_buf[recvnum]='\0';

        if (0==strcmp(recv_buf,"quit"))
        {
            perror("quitting remote controling\n");
            flag = 1;

        }
        printf("the message is: %s\n",recv_buf);

        strcpy(cmd,"/bin/");
        strcat(cmd,recv_buf);
        execute(cmd,send_buf);

        if ('\0'==*send_buf)
        {
            memset(cmd,0,sizeof(cmd));
            strcpy(cmd,"/sbin/");
            strcat(cmd,recv_buf);
            execute(cmd,send_buf);

            if ('\0'==*send_buf)
            {
                memset(cmd,0,sizeof(cmd));
                strcpy(cmd,"/usr/bin/");
                strcat(cmd,recv_buf);
                execute(cmd,send_buf);
            }

            if ('\0'==*send_buf)
            {
                memset(cmd,0,sizeof(cmd));
                strcpy(cmd,"/usr/sbin/");
                strcat(cmd,recv_buf);
                execute(cmd,send_buf);
            }
        }
        if ('\0'==*send_buf)
            sprintf(send_buf,"command is not vaild,check it please\n");

        printf("the server message is:%s\n",send_buf);

        if (0>send(connectd,send_buf,sizeof(send_buf),0))
        {
            perror("sending data error\n");
        }

        if(flag==1){
            pthread_exit(&flag_exit);
        }
    }
}
int main()
{
    // pthread_t connect[5];
    pthread_t thread;
    int connectd;
    port = PORT;
    opt = SO_REUSEADDR;


    if (-1==(listend=socket(AF_INET,SOCK_STREAM,0)))
    {
        perror("generating socket error\n");
        exit(1);

    }
    setsockopt(listend,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));

    memset(&server,0,sizeof(struct sockaddr_in));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(port);

    if (-1==bind(listend,(struct sockaddr*)&server,sizeof(struct sockaddr)))
    {
        perror("binding error\n");
        close(listend);
        exit(1);

    }
    if (-1==listen(listend,5))
    {
        perror("listen error\n");
        exit(1);
    }


    while(1)
    {

        memset(send_buf,0,2048);
        memset(recv_buf,0,2048);
        if (-1==(connectd=accept(listend,(struct sockaddr*)&client,&len)))
        {
            perror("create connect socket error\n");
            continue;
        }
        if (-1==(flag_exit=pthread_create(&thread,NULL ,(void*)test,(void*)connectd))){
            perror("can not create thread");
            exit(1);
        }
        continue;
        close(listend);
        exit(1);
    }
}

int execute(char* command,char* buf)
{
    FILE * 	fp;
    int count;

    if (NULL==(fp = popen(command,"r")))
    {
        perror("creating pipe error\n");
        exit(1);

    }

    count = 0 ;

    while(((buf[count] = fgetc(fp))!=EOF)&&count<2047)
        count++;
    buf[count]='\0';

    pclose(fp);
    return count;


}



