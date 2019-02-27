
#include<stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef __linux__
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <netinet/ip_icmp.h>
#include <net/if_arp.h>
#include <netinet/if_ether.h>
#include <net/if.h>
#include <sys/ioctl.h>
#elif __win32__
#include <windows.h>

#endif

void UnpackARP(char *buff);
void UnpackIP(char *buff);
void UnpackTCP(char *buff);
void UnpackUDP(char *buff);
void UnpackICMP(char *buff);
void UnpackIGMP(char *buff);

int main(int argc, char **argv){
    int sockfd, i;
    char buff[2048];

    if(0>(sockfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL)))){
        perror("socket");
        exit(-1);
    }

    while(1){
        memset(buff, 0, 2048);
        int n = recvfrom(sockfd, buff, 2048, 0, NULL, NULL);

        printf("start ===========\n");
        printf("size: %d\n", n);
        struct ethhdr *eth = (struct ethhdr*)buff;
        char *nextStack = buff + sizeof(struct ethhdr);
        int protocol = ntohs(eth->h_proto);
        switch(protocol) {
            case ETH_P_IP:
                UnpackIP(nextStack);
                break;
            case ETH_P_ARP:
                UnpackARP(nextStack);
                break;
        }
        printf("finish =================\n\n");
    }
    return 0;
}

void getAddress(long saddr, char *str) {
    sprintf(str, "%d.%d.%d.%d", \
            ((unsigned char*)&saddr)[0], \
            ((unsigned char*)&saddr)[1], \
            ((unsigned char*)&saddr)[2], \
            ((unsigned char*)&saddr)[3]);
}

void UnpackARP(char *buff) {
    printf("ARP data packet\n");
}

void UnpackIP(char *buff) {
    struct iphdr *ip = (struct iphdr*)buff;
    char *nextStack = buff + sizeof(struct iphdr);
    int protocol = ip->protocol;
    char data[20];

    getAddress(ip->saddr, data);
    printf("source ip %s\n", data);
    bzero(data, sizeof(data));

    getAddress(ip->daddr, data);
    printf("destination ip %s\n", data);

    switch(protocol) {
        case 0x06:
            UnpackTCP(nextStack);
            break;
        case 0x17:
            UnpackUDP(nextStack);
            break;
        case 0x01:
            UnpackICMP(nextStack);
            break;
        case 0x02:
            UnpackIGMP(nextStack);
            break;
        default:
            printf("unknown protocol\n");
            break;
    }
}

void UnpackTCP(char *buff) {
    struct tcphdr *tcp = (struct tcphdr*)buff;
    printf("transport layer protocol:tcp\n");
    printf("source port:%d\n", ntohs(tcp->source));
    printf("destination port:%d\n", ntohs(tcp->dest));
}
void UnpackUDP(char *buff) {
    struct udphdr *udp = (struct udphdr*)buff;
    printf("transport layer protocol:udp\n");
    printf("source port:%d\n", ntohs(udp->source));
    printf("destination port:%d\n", ntohs(udp->dest));
}
void UnpackICMP(char *buff) {
    printf("ICMP data packet\n");
}
void UnpackIGMP(char *buff) {
    printf("IGMP data packet\n");
}
