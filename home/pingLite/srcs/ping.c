/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danysousa <danysousa@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2015/11/23 10:33:21 by danysousa         #+#    #+#             */
/*   Updated: 2015/12/01 12:31:14 by danysousa        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ping.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/socket.h>
#include <resolv.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip_icmp.h>
#include <strings.h>



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


void			display(void *buff/*, t_info *info*/)
{
	struct iphdr	*ip = buff;
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

	printf("*****************************\n%d bytes from %s: icmp_seq=%d ttl=%d time=%d ms\n",
	 ntohs(ip->tot_len) - (ip->ihl * 4),
	 src,
	 0,
	 ip->ttl,
	 42);
}

int		init_sock()
{
	int			sd;
	const int	val = 64;

	sd = socket(PF_INET, SOCK_RAW, IPPROTO_ICMP);
	if ( sd < 0 )
	{
		perror("socket");
		return (-1);
	}
	if ( setsockopt(sd, SOL_IP, IP_TTL, &val, sizeof(val)) != 0)
	{
		perror("Set TTL option");
		return (-1);
	}

	return (sd);
}

struct sockaddr_in		*get_sock_info(char *adress)
{
	struct addrinfo			*addrinfo;
	struct sockaddr_in		*addr_ping;

	addr_ping = malloc(sizeof(struct sockaddr_in));
	ft_bzero(addr_ping, sizeof(addr_ping));
	getaddrinfo(adress, NULL, NULL, &addrinfo);
	addr_ping->sin_family = addrinfo->ai_family;
	addr_ping->sin_port = 0;
	addr_ping->sin_addr.s_addr = ((struct sockaddr_in *)addrinfo->ai_addr)->sin_addr.s_addr;

	return (addr_ping);
}

int					loop(t_contact *contact, int cnt)
{
	socklen_t			len;
	size_t				i;

	len = sizeof(*contact->recv);
	ft_bzero(&contact->pckt, sizeof(contact->pckt));
	contact->pckt.hdr.type = ICMP_ECHO;
	contact->pckt.hdr.un.echo.id = contact->pid;
	for ( i = 0; i < sizeof(contact->pckt.msg) - 1; i++ )
		contact->pckt.msg[i] = i + '0';
	contact->pckt.msg[i] = 0;
	contact->pckt.hdr.un.echo.sequence = cnt;
	contact->pckt.hdr.checksum = checksum(&contact->pckt, sizeof(contact->pckt));
	if ( sendto(contact->sd, &contact->pckt, sizeof(contact->pckt), 0, (struct sockaddr*)contact->send, sizeof(*contact->send)) <= 0 )
	{
		ft_putendl_fd("Error with send packet", 2);
		return (-1);
	}

	if ( recvfrom(contact->sd, &contact->pckt, sizeof(contact->pckt), 0, (struct sockaddr*)contact->recv, &len) < 0 )
	{
		ft_putendl_fd("Error with recv packet", 2);
		return (-2);
	}
	return (0);
}

int						ping(char *adress, int pid)
{
	t_contact				contact;
	int						cnt;

	cnt = 1;
	contact.send = get_sock_info(adress);
	contact.recv = malloc(sizeof(struct sockaddr_in));
	contact.sd = init_sock();
	contact.pid = pid;

	while (0x2A)
	{
		loop(&contact, cnt);
		cnt++;
		display(&contact.pckt);
		sleep(1);
	}

	return 1;
}

/*--------------------------------------------------------------------*/
/*--- main - look up host and start ping processes.                ---*/
/*--------------------------------------------------------------------*/
int main(int argc, char *argv[])
{
	int		pid;

	pid = getpid();
	if (argc < 2)
	{
		ft_putendl_fd("Usage: ./ping [hostname/ip]", 2);
		return (-1);
	}
	// if (pid != 0)
	// {
	// 	ft_putendl_fd("Sorry, ft_ping required root rights", 2);
	// 	return (-2);
	// }

	return ping(argv[1], pid);
}

