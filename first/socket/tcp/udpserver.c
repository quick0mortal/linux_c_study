#include<stdlib.h>
#include<stdio.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>
#include<unistd.h>
#include<netinet/in.h>
#include<arpa/inet.h>

#define PORT 8900

int main(int argc,char** argv)
{
    struct sockaddr_in server;
    struct sockaddr_in client;
    int len;
    int port;
    int listend;
    int connectd;
    int sendnum;
    int opt;
    int recvnum;
    char send_buf[2048];
    char recv_buf[2048];

    //if (2==argc)

    port= PORT;
    memset(send_buf,0,2048);
    memset(recv_buf,0,2048);

    opt = SO_REUSEADDR;


    while(1)
    {
        if (-1==(listend=socket(AF_INET,SOCK_DGRAM,0)))
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


#ifdef DEBUG
        printf("the connect id is %d",listend);
        printf("the client ip addr is %s",inet_ntoa(client.sin_addr));
#endif

        sendnum = sprintf(send_buf,"hello,the guest from %s\n",inet_ntoa(client.sin_addr));
        sendto(listend, send_buf, sizeof(send_buf), 0, (struct sockaddr_in *)&client,sizeof(client));

#ifdef DEBUG
        printf("the send num is %d",sendnum);
        printf("the client ip addr is %s",inet_ntoa(client.sin_addr));
#endif

        recvnum = recvfrom(listend, recv_buf, sizeof(recv_buf), 0, (struct sockaddr_in *)&client, sizeof(client));
        if(recvnum == -1)
        {
            perror("Failed to recvfrom.");
            break;
        }
        recv_buf[recvnum]='\0';

        printf ("the message from the client is: %s\n",recv_buf);

        if (0==strcmp(recv_buf,"quit"))
        {
            perror("the client break the server process\n");
            close(listend);
        }

        sendnum = sprintf(send_buf,"byebye,the guest from %s\n",inet_ntoa(client.sin_addr));
        sendnum = sendto(listend,send_buf,sizeof(send_buf), 0, (struct sockaddr_in *)&server, sizeof(server));

        if(sendnum == -1)
        {
            perror("Fiailed to sendto.");
        }
        close(listend);
        continue;

    }

    return 0;

}




