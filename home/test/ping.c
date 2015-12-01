#include <fcntl.h>
#include <errno.h>
#include <sys/socket.h>
#include <resolv.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip_icmp.h>
#include <strings.h>


#define PACKETSIZE  64
struct packet
{
    struct icmphdr hdr;
    char msg[PACKETSIZE-sizeof(struct icmphdr)];
};

int pid=-1;
struct protoent *proto=NULL;
int cnt=1;

/*--------------------------------------------------------------------*/
/*--- checksum - standard 1s complement checksum                   ---*/
/*--------------------------------------------------------------------*/
unsigned short checksum(void *b, int len)
{
    unsigned short *buf = b;
    unsigned int sum=0;
    unsigned short result;

    for ( sum = 0; len > 1; len -= 2 )
        sum += *buf++;
    if ( len == 1 )
        sum += *(unsigned char*)buf;
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    result = ~sum;
    return result;
}


void        display(void *buff/*, t_info *info*/)
{
    struct iphdr    *ip = buff;
    char src[INET_ADDRSTRLEN];
    char dest[INET_ADDRSTRLEN];

    inet_ntop( AF_INET, (void *)&ip->saddr, src, sizeof(src) );
    inet_ntop( AF_INET, (void *)&ip->daddr, dest, sizeof(dest) );

    printf("IPv%d: hdr-size=%d pkt-size=%d protocol=%d TTL=%d src=%s dest=%s\n",
        ip->version,
        ip->ihl*4,
        ntohs(ip->tot_len),
        ip->protocol,
        ip->ttl,
        src,
        dest);

    // printf("*****************************\n%d bytes from %s: icmp_seq=%d ttl=%d time=%d ms\n",
    //  ntohs(ip->tot_len),
    //  info->ip_dest,
    //  0,
    //  ip->ttl,
    //  42);
}

/*--------------------------------------------------------------------*/
/*--- ping - Create message and send it.                           ---*/
/*    return 0 is ping Ok, return 1 is ping not OK.                ---*/
/*--------------------------------------------------------------------*/
int ping(char *adress)
{
    const int val=64;
    int i, sd;
    struct packet pckt;
    struct sockaddr_in r_addr;
    int loop;
    // struct hostent *hname;
    struct addrinfo *addrinfo;
    struct sockaddr_in addr_ping,*addr;

    pid = getpid();
    getaddrinfo(adress, NULL, NULL, &addrinfo);
    bzero(&addr_ping, sizeof(addr_ping));
    addr_ping.sin_family = addrinfo->ai_family;
    addr_ping.sin_port = 0;
    addr_ping.sin_addr.s_addr = ((struct sockaddr_in *)addrinfo->ai_addr)->sin_addr.s_addr;

    addr = &addr_ping;

    sd = socket(PF_INET, SOCK_RAW, IPPROTO_ICMP);
    if ( sd < 0 )
    {
        perror("socket");
        return 1;
    }
    if ( setsockopt(sd, SOL_IP, IP_TTL, &val, sizeof(val)) != 0)
    {
        perror("Set TTL option");
        return 1;
    }

    while (0x2A)
    {

        int len=sizeof(r_addr);

        bzero(&pckt, sizeof(pckt));
        pckt.hdr.type = ICMP_ECHO;
        pckt.hdr.un.echo.id = pid;
        for ( i = 0; i < sizeof(pckt.msg)-1; i++ )
            pckt.msg[i] = i+'0';
        pckt.msg[i] = 0;
        pckt.hdr.un.echo.sequence = cnt++;
        pckt.hdr.checksum = checksum(&pckt, sizeof(pckt));
        if ( sendto(sd, &pckt, sizeof(pckt), 0, (struct sockaddr*)addr, sizeof(*addr)) <= 0 )
            perror("sendto");

        if ( recvfrom(sd, &pckt, sizeof(pckt), 0, (struct sockaddr*)&r_addr, &len) < 0 )
        {
            return -1;
        }

        display(&pckt);
        sleep(1);

    }

    return 1;
}

/*--------------------------------------------------------------------*/
/*--- main - look up host and start ping processes.                ---*/
/*--------------------------------------------------------------------*/
int main(int argc, char *argv[])
{
    if (argc < 2)
        return (-1);

    if (ping(argv[1]))
        printf("Ping is not OK. \n");
    else
        printf("Ping is OK. \n");


    return 0;
}

