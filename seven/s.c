#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#define PORT 9876
#define BUF_SIZE 2048
struct Client
{
    int fd; ;//客户端连接套接字
    char name[256]; //客户的名字
    int first; ;//用于只是用户是否刚刚登陆
};
int main()
{
    int listend;
    int connected;
    int tempid;
    int sendnum;
    int recvnum;
    char send_buf[2048];
    char recv_buf[2048];
    char command[2048];
    int ret;
    char buf[BUF_SIZE];
    struct sockaddr_in server;
    struct sockaddr_in client;
    int count;//计数
    int counter = 0;
    int num;//计数已经准备好的I/O设备
    int maxfd;//存储最大的文件描述符
    fd_set rset;
    fd_set allset;
    int backnum;
    int length;
    int port;
    int opt;
    int i,j,k;//局部变量
    port =PORT;
    opt = SO_REUSEADDR;
    //侦听
    if ( -1 == (listend = socket(AF_INET,SOCK_STREAM,0)))
    {
        perror("create socket error !\n");
        return -1;
    }
    setsockopt(listend,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));
    memset(&server,0,sizeof(struct sockaddr_in));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(port);
    //绑定
    if (-1 == bind(listend, (struct sockaddr*) &server, sizeof(struct sockaddr)))
    {
        perror("binding socket error !\n");
        close(listend);
        return -1;
    }
    backnum = 5;
    if (-1 == listen(listend,5))
    {
        perror("listening socket error !\n");
        close(listend);
        return -1;
    }

    length = sizeof(struct sockaddr);
    maxfd=listend;
    counter=-1;
    FD_ZERO(&allset);
    FD_SET(listend,&allset);
    struct Client clientInfo[FD_SETSIZE];
    for(count = 0;count < FD_SETSIZE; count++)
    {
        clientInfo[count].fd = -1;
        memset(clientInfo[count].name,0,256);
        clientInfo[count].first = -1;
    }
    while(1)
    {
        rset = allset;
        num = select(maxfd+1,&rset,NULL,NULL,NULL);

        if (FD_ISSET(listend, &rset))
        {
            if (-1== (connected = accept(listend,(struct sockaddr*)&client,&length )))
            {
                perror("accepting error\n");
                continue;
            }
            for(count = 0; count<FD_SETSIZE; count++)
            {
                if (-1 == clientInfo[count].fd)
                {
                    clientInfo[count].fd = connected;
                    break;
                }
            }
            FD_SET(connected,&allset);
            if (connected > maxfd)
                maxfd = connected;
            if (count > counter)
                counter = count;
            if (--num <= 0)
                continue;
        }
        for(count = 0; count <= counter; count++)
        {
            if( 0 > (tempid = clientInfo[count].fd))
                continue;
            if(FD_ISSET(tempid,&rset))
            {
                memset(send_buf,0,2048);
                memset(recv_buf,0,2048);
                memset(command,0,2048);
                if ( 0 >= (recvnum = recv(tempid,recv_buf,sizeof(recv_buf),0)))
                {
                    perror("receiving information error\n");
                    close(tempid);
                    FD_CLR(tempid,&allset);
                    clientInfo[count].fd=-1;
                    clientInfo[count].first = -1;
                    memset(clientInfo[count].name,0,256);
                    continue;
                }
                recv_buf[recvnum]='\0';
                if(clientInfo[count].first == -1) //用户首次登陆
                {
                    strcpy(clientInfo[count].name,recv_buf);
                    printf("%s enters\n",clientInfo[count].name);
                    clientInfo[count].first = 1;
                    continue;
                }
                else
                {
                    if(recv_buf[0] == '/')
                    {
                        if(recv_buf[1] == 'q') //用户退出聊天室
                        {
                            memset(send_buf,0,2048);
                            memset(recv_buf,0,2048);
                            sprintf(send_buf,"%s quits\n", clientInfo[count].name);
                            close(clientInfo[count].fd);
                            clientInfo[count].fd = -1;
                            memset(clientInfo[count].name,0,256);
                            clientInfo[count].first = -1;
                            for(i = 0; i < counter+1; i++)
                            {

                                if(strcmp(clientInfo[i].name,clientInfo[count].name) == 0)
                                    continue;
                                else
                                    send(clientInfo[i].fd,send_buf,sizeof(send_buf),0);
                            }
                        }
                        else if(recv_buf[1] == 'w')//显示在线用户
                        {
                            for(i = 0; i <= counter; i++)
                            {
                                if(clientInfo[i].fd < 0)
                                    continue;
                                strcat(send_buf,clientInfo[i].name);
                                strcat(send_buf,"\n");
                            }
                            send(tempid,send_buf,sizeof(send_buf),0);
                        }
                        else if(recv_buf[1] == 's')//向指定用户发送消息
                        {
                            char name[256];
                            char msg[256];
                            for(i = 3; i < strlen(recv_buf);i++)
                            {
                                if(recv_buf[i] == ' ')
                                {
                                    break;
                                }
                                name[i-3] = recv_buf[i];
                            }
                            name[i-3] = '\0';
                            k= 0 ;
                            for( j = i+1; j < strlen(recv_buf); j++)
                            {
                                msg[k] = recv_buf[j];
                                k++;
                            }
                            msg[k+1] = '\0';
                            for(i = 0; i < counter+1; i++)
                            {
                                if(strcmp(clientInfo[i].name,name)==0)
                                {
                                    send(clientInfo[i].fd,msg,sizeof(msg),0);
                                }
                            }
                        }
                    }
                    else
                    {
                        for(i = 0; i < counter+1; i++)
                        {
                            // if(clientInfo[i].fd < 0)
                            // {
                            // 	continue;
                            // }

                            if(strcmp(clientInfo[i].name,clientInfo[count].name) == 0)
                                continue;
                            else
                                send(clientInfo[i].fd,recv_buf,sizeof(recv_buf),0);

                        }
                    }
                }
            }
        }
    }
    close(listend);
}
