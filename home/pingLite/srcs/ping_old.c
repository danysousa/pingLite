/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping_old.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danysousa <danysousa@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2015/11/23 10:33:21 by danysousa         #+#    #+#             */
/*   Updated: 2015/12/01 11:45:25 by danysousa        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ping.h>

/*
** FOR DEBUG
*/
#include <stdio.h>

unsigned short		checksum(void *b, int len)
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

/*
** INIT ADDRINFO STRUCT
*/
int					init_addrinfo(char *host, struct addrinfo **addrinfo)
{
	struct addrinfo hints;

	ft_bzero(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = 0;
	hints.ai_protocol = 0;
	hints.ai_flags = 0;

	if (getaddrinfo(host, NULL, &hints, addrinfo) != 0)
	{
		ft_putstr_fd("Cannot resolve ", 2);
		ft_putendl_fd(host, 2);
		return (-1);
	}

	return (0);
}

/*
** INIT SOCKET
*/
int						init_scoket(struct addrinfo *addrinfo)
{
	unsigned short		val;
	int					sock;

	val = 64;
	// addrinfo->ai_family = AF_INET;
	sock = socket(addrinfo->ai_family, SOCK_RAW, IPPROTO_ICMP);
	if (sock < 0)
	{
		ft_putendl_fd("Socket init failed", 2);
		return (-1);
	}

	if ( setsockopt(sock, SOL_IP, IP_TTL, (void *)&val, sizeof(val)) != 0)
	{
		ft_putendl_fd("Set socket opt failed", 2);
		return (-2);
	}

	return (sock);
}


/*
** DISPLAY
*/

t_info		*init_info_struct(struct addrinfo *a_info, char *host)
{
	t_info				*info;
	struct addrinfo		*_res;
	int					sock;

	info = malloc(sizeof(t_info));
	for (_res = a_info; _res != NULL; _res = _res->ai_next)
	{
		sock = socket(_res->ai_family, SOCK_RAW, IPPROTO_ICMP);
		if (sock == -1)
			continue ;

		inet_ntop( AF_INET, &((struct sockaddr_in *)_res->ai_addr)->sin_addr, info->ip_dest, sizeof(info->ip_dest) );
		ft_putendl(info->ip_dest);
		info->ai = _res;
		if (bind(sock, _res->ai_addr, _res->ai_addrlen) == 0)
			break ;
		close(sock);
	}

	info->host_dest = host;
	return (info);
}

void		display(void *buff/*, t_info *info*/)
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

	// printf("*****************************\n%d bytes from %s: icmp_seq=%d ttl=%d time=%d ms\n",
	// 	ntohs(ip->tot_len),
	// 	info->ip_dest,
	// 	0,
	// 	ip->ttl,
	// 	42);
}


int						main(int argc, char *argv[])
{
	struct addrinfo		*a_info;
	int					sock;
	t_info				*info;

	if (argc < 2)
	{
		ft_putendl_fd("Usage: ./ping [hostname/ip]", 2);
		return (-1);
	}

	if (init_addrinfo(argv[1], &a_info) == -1)
		return (-2);
	info = init_info_struct(a_info, argv[1]);
	sock = init_scoket(info->ai);
	if ( sock < 0 )
		return (sock);

	/*
	** SEND / RECEIVE MSG
	*/
	size_t					i;
	t_ping					pack;
	int						count = 1;
	struct iphdr			buff;

	while (TRUE)
	{
		ft_bzero(&pack, sizeof(pack));
		ft_bzero((void *)&buff, sizeof(buff));

		pack.head.type = 8;
		pack.head.code = count;
		pack.id = count++;
		if (bind(sock, info->ai->ai_addr, info->ai->ai_addrlen) == 0)
			ft_putendl("BIND");
		while (i < 15)
		{
			pack.seq[i] = i + '0';
			i++;
		}
		pack.seq[i] = 0;
		pack.head.chk = checksum(&pack, sizeof(pack));
		if (sendto(sock, &pack, sizeof(pack), 0, info->ai->ai_addr, info->ai->ai_addrlen) < 0)
			perror("sendto");
		if (recvfrom(sock, (void *)&buff, sizeof(buff), 0, info->ai->ai_addr, &info->ai->ai_addrlen) < 0)
			perror("recvfrom");

		display((void *)&buff);
		sleep(1);
	}

	return 0;
}

