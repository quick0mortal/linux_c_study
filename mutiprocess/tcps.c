#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <signal.h>
// #define PORT 8900
#define BUF_SIZE 2048

int execute(char* command,char* buf);

void print_usage(char * cmd)
{
	fprintf(stderr," %s usage:\n",cmd);
	fprintf(stderr,"%s port\n",cmd);

}

void sig_child(int signo){
      pid_t  pid;
      int  stat,i;
      while((pid = waitpid(-1, &stat, WNOHANG)) > 0);
      return;
}

int refun(int connectd, int listend, int recvnum)
{
    char send_buf[2048] = {'\0'};
	char recv_buf[2048] = {'\0'};
    char cmd[2048];
    int tempnum;
    int sendnum;

    // memset(send_buf,0,2048);
	// memset(recv_buf,0,2048);

       tempnum = (recvnum = recv(connectd,recv_buf,sizeof(recv_buf),0));
       printf("%d\n",tempnum);
       if (0>tempnum)
	{
		perror("recv error\n");
		close(connectd);
		return 0;
	}else if(tempnum == 0) {

    }
	recv_buf[recvnum]='\0';

	printf ("the message from the client is: %s\n",recv_buf);

	if (recv_buf[0] == 'q' && recv_buf[1] == 'u' && recv_buf[2] == 'i' && recv_buf[3] == 't')
    {
        perror("quit\n");
		close(connectd);
		return 0;
    }

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

    printf("%s",send_buf);
    sendnum = sprintf(send_buf,send_buf);
       if ( 0 >send(connectd,send_buf,sendnum,0))
	{
		perror("send error\n");
		close(connectd);
		return 0;
	}

	// close(connectd);
	return 0;
}

int main(int argc,char** argv)
{
    signal(SIGCHLD,sig_child);
	struct sockaddr_in server;
	struct sockaddr_in client;
	int len;
	int port;
	int listend;
	int connectd;
	int opt;
	int recvnum;
    int status;
    char send_data[2048];
    pid_t pid;

       if (2==argc)
       {
		port = atoi(argv[1]);
       }else
       {
        print_usage(argv[0]);
		exit(1);
       }


      opt = SO_REUSEADDR;


      if (-1==(listend=socket(AF_INET,SOCK_STREAM,0)))
      {
	 perror("create listen socket error\n");
	 exit(1);
      }
      setsockopt(listend,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));

      #ifdef DEBUG
	printf("the listen id is %d\n",listend);
      #endif

     memset(&server,0,sizeof(struct sockaddr_in));
     server.sin_family = AF_INET;
     server.sin_addr.s_addr = htonl(INADDR_ANY);
     server.sin_port = htons(port);

     if (-1==bind(listend,(struct sockaddr *)&server,sizeof(struct sockaddr)))
     {
	perror("bind error\n");
	exit(1);

     }

    if (-1==listen(listend,5))
    {
	perror("listen error\n");
	exit(1);
    }


while(1) {
    if (-1==(connectd=accept(listend,(struct sockaddr*)&client,&len)))

    	{
		perror("create connect socket error\n");
		exit(0);
    	}

    pid = fork();

	if(0>pid)
	{
		perror("fork error\n");
		exit(1);

	}
	else if(0==pid)
	{
        refun(connectd, listend, recvnum);
        _exit(0);
	}
	else
	{
        close(connectd);
		// waitpid(pid, &status, 0 );
	}
}


    close(listend);
    return 0;



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



