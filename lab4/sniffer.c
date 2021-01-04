#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <netinet/ip_icmp.h>
#include <net/if_arp.h>
#include <linux/if_packet.h>
#include <netinet/if_ether.h>
#include <netinet/ether.h>
#include <net/if.h>
#include <sys/ioctl.h>

void UnpackIP(char *buff);
void UnpackTCP(char *buff);
void UnpackUDP(char *buff);

int main()
{
    int sockfd, i;
    char buff[2048];

    if (0 > (sockfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL))))
    {
        perror("socket");
        exit(-1);
    }

    while (1)
    {
        memset(buff, 0, 2048);

        int size = recvfrom(sockfd, buff, 2048, 0, NULL, NULL);
        if (size < 42)
        {
            fprintf(stdout, "Incomplete header, packet corrupt\n");
            continue;
        }

        printf("开始解析数据包============\n");
        printf("大小: %d\n", size);

        struct ethhdr *eth = (struct ethhdr *)buff;

        // 取低8位
        int n = 0XFF;
        char* sourceMac = eth->h_source;
        char* destMac = eth->h_dest;
        printf("MAC: %.2X:%02X:%02X:%02X:%02X:%02X==>"
               "%.2X:%.2X:%.2X:%.2X:%.2X:%.2X\n",
               sourceMac[0] & n, sourceMac[1] & n, sourceMac[2] & n, sourceMac[3] & n, sourceMac[4] & n, sourceMac[5] & n,
               destMac[0] & n, destMac[1] & n, destMac[2] & n, destMac[3] & n, destMac[4] & n, destMac[5] & n);

        // 解析数据部分
        char *nextStack = buff + sizeof(struct ethhdr);
        int protocol = ntohs(eth->h_proto);
        switch (protocol)
        {
            case ETH_P_IP:
                UnpackIP(nextStack);
                break;
            case ETH_P_ARP:
                printf("ETH_P_ARP\n");
                break;
        }
        printf("解析结束=================\n\n");
        getchar();
    }
    return 0;
}

void getAddress(long saddr, char *str)
{
    sprintf(str, "%d.%d.%d.%d",
            ((unsigned char *)&saddr)[0],
            ((unsigned char *)&saddr)[1],
            ((unsigned char *)&saddr)[2],
            ((unsigned char *)&saddr)[3]);
}

void UnpackIP(char *buff)
{
    struct iphdr *ip = (struct iphdr *)buff;
    char *nextStack = buff + sizeof(struct iphdr);
    int protocol = ip->protocol;
    char data[20];

    getAddress(ip->saddr, data);
    printf("IP: %s -> ", data);
    bzero(data, sizeof(data));

    getAddress(ip->daddr, data);
    printf("%s\n", data);

    switch (protocol)
    {
        case IPPROTO_TCP:
            UnpackTCP(nextStack);
            break;
        case IPPROTO_UDP:
            UnpackUDP(nextStack);
            break;
        case IPPROTO_ICMP:
            printf("IPPROTO_ICMP\n");
            break;
        case IPPROTO_IGMP:
            printf("IPPROTO_IGMP\n");
            break;
        case IPPROTO_IPV6:
            printf("IPPROTO_IPV6\n");
            break;
        case IPPROTO_IPIP:
            printf("IPPROTO_IPIP\n");
            break;
        default:
            printf("unknown protocol\n");
            break;
    }
}

void UnpackTCP(char *buff)
{
    struct tcphdr *tcp = (struct tcphdr *)buff;
    printf("传输层:tcp   ");
    printf("端口:%d -> ", ntohs(tcp->source));
    printf("%d\n", ntohs(tcp->dest));
}
void UnpackUDP(char *buff)
{
    struct udphdr *udp = (struct udphdr *)buff;
    printf("传输层:udp   ");
    printf("端口:%d -> ", ntohs(udp->source));
    printf("%d\n", ntohs(udp->dest));
}