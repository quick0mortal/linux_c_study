#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <netinet/if_ether.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <pcap.h>
#include <netdb.h>
#include <time.h>
#include <sys/time.h>
#include <ctype.h>

#define DEFAULT_SNAPLEN 1518
void print(u_char *payload, int len, int offset, int maxlen)
{
    printf("%.5d  ", offset);
    int max = maxlen;
    int i;
    for (i = 0; i < 16; i++)
    {
        if ((len - i) > 0)
        {
            printf("%.2x ", payload[max - (len - i)]);
        }
        else
        {
            printf("   ");
        }
    }
    printf("\t");
    for (i = 0; i < 16; i++)
    {
        if (isprint(payload[max - (len - i)]))
        {
            printf("%c", payload[max - (len - i)]);
        }
        else
        {
            printf(".");
        }
    }
}

void print_data(u_char *payload, int len)
{
    int line_width = 16;
    int len_rem = len;
    int maxlen = len;
    int offset = 0;
    while (1)
    {
        if (len_rem < line_width)
        {
            if (len_rem == 0)
            {
                break;
            }
            else
            {
                print(payload, len_rem, offset, maxlen);
                offset = offset + len_rem;
                printf("\n");
                break;
            }
        }
        else
        {
            print(payload, len_rem, offset, maxlen);
            offset = offset + 16;
            printf("\n");
        }
        len_rem = len_rem - line_width;
    }
}

void print_mac(u_char *macadd)
{
    int i;
    for (i = 0; i < 5; i++)
    {
        printf("%.2x:", macadd[i]);
    }
    printf("%.2x", macadd[i]);
}

void print_ip(u_char *ipadd)
{
    int i;
    for (i = 0; i < 3; ++i)
    {
        printf("%d.", ipadd[i]);
    }
    printf("%d", ipadd[i]);
}

void packet_process(u_char *user, const struct pcap_pkthdr *h, const u_char *p)
{
    struct ether_header *eth;
    static long int packet_num = 0;
    struct ether_arp *arppkt;
    struct ip *iph;
    struct icmphdr *icmp;
    struct tcphdr *tcph;
    struct udphdr *udph;
    int m;
    char *buf;
    printf("-----------------------------------------------------\n");
    printf("Packet capture\n");
    printf("Recieved at %s\n", ctime((const time_t *)&(h->ts).tv_sec));
    printf("Packet  number:%ld\n", ++packet_num);
    printf("Packet  length::%d\n", h->len);
    int i;
    eth = (struct ether_header *)p;
    printf("Source Mac Address: ");
    print_mac(eth->ether_shost);
    printf("\n");
    printf("Destination Mac Address:");
    print_mac(eth->ether_dhost);
    printf("\n");
    unsigned int typeno;
    typeno = ntohs(eth->ether_type);
    switch (typeno)
    {
        case ETHERTYPE_IP:
            printf("IPV4\n");
            break;
        case ETHERTYPE_PUP:
            printf("PUP\n");
            break;
        case ETHERTYPE_ARP:
            printf("ARP\n");
            break;
        default:
            printf("unknown network layer types\n");
    }
    if (typeno == ETHERTYPE_IP)
    {
        iph = (struct ip *)(p + sizeof(struct ether_header));

        printf("Source Ip Address:");
        print_ip((u_char *)&(iph->ip_src));
        printf("\n");

        printf("Destination Ip address:");
        print_ip((u_char *)&(iph->ip_dst));
        printf("\n");
        printf("Transport layer protocal:");
        if (iph->ip_p == 1) //判断传输层协议
        {
            printf("ICMP\n");
        }
        else if (iph->ip_p == 2)
        {
            printf("IGMP\n");
        }
        else if (iph->ip_p == 6)
        {
            printf("TCP\n");
            tcph = (struct tcphdr *)(p + sizeof(struct ether_header) + sizeof(struct ip));

            printf("destport :%d\n", ntohs(tcph->dest));
            printf("sourport:%d\n", ntohs(tcph->source));
            printf("Payload:\n");
            print_data(p, h->len);
        }
        else if (iph->ip_p == 17)
        {
            printf("UDP\n");
            // printf("application layer protocol:\n");
            udph = (struct udphdr *)(p + sizeof(struct ether_header) + sizeof(struct ip));

            printf("dest port:%d\n", ntohs(udph->dest));
            printf("source port%d\n:", ntohs(udph->source));
            printf("Payload:\n");
            print_data(p, h->len);
        }
        else
        {
            printf("unknown protocol\n");
        }
    }
}

int main(int argc, char **argv)
{
    char ebuf[PCAP_ERRBUF_SIZE];
    pcap_t *pd;
    struct bpf_program fcode;
    bpf_u_int32 netaddr = 0, mask = 0;
    int i = 0;

    if (argc <= 1)
    {
        printf("usage:%s<network interface\n>", argv[0]);
        exit(1);
    }

    //open the port and set
    if ((pd = pcap_open_live(argv[1], DEFAULT_SNAPLEN, 1, 1000, ebuf)) == NULL)
    {
        (void)fprintf(stderr, "%s", ebuf);
    }
    //find the netaddr and mask
    if (i = pcap_lookupnet(argv[1], &netaddr, &mask, ebuf) == -1)
    {
        (void)fprintf(stderr, "%s", ebuf);
    }
    //set the filter

    pcap_compile(pd, &fcode, "ip", 1, mask);

    pcap_setfilter(pd, &fcode);

    //get the packet
    if (pcap_loop(pd, -1, packet_process, NULL) < 0)
    {
        (void)fprintf(stderr, "pcap_loop:%s\n", pcap_geterr(pd));
    }

    pcap_close(pd);
}
