/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danysousa <danysousa@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2015/11/23 10:33:21 by danysousa         #+#    #+#             */
/*   Updated: 2015/11/24 16:50:39 by danysousa        ###   ########.fr       */
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

int						main(int argc, char *argv[])
{
	struct addrinfo		*addrinfo;
	int					sock;

	if (argc < 2)
	{
		ft_putendl_fd("Usage: ./ping [hostname/ip]", 2);
		return (-1);
	}

	/*
	** INIT ADDRINFO STRUCT
	*/
	if (getaddrinfo(argv[1], NULL, NULL, &addrinfo) != 0)
	{
		ft_putstr_fd("Cannot resolve ", 2);
		ft_putendl_fd(argv[1], 2);
		return (-2);
	}

	/*
	** INIT SOCKET
	*/

/*
** A voir sur une VM Linux
** sock = socket(addrinfo->ai_family, SOCK_RAW, 1);
*/
	int val;

	val = 255;
	addrinfo->ai_family = PF_INET;
	sock = socket(addrinfo->ai_family, SOCK_RAW, 1);
	// sock = socket(addrinfo->ai_family, addrinfo->ai_socktype, addrinfo->ai_protocol);
	if (sock < 0)
	{
		ft_putendl_fd("Socket init failed", 2);
		return (-3);
	}

	ft_putendl("That's good");

	if ( setsockopt(sock, SOL_SOCKET, IP_TTL, (void *)&val, sizeof(val)) != 0)
	{
		ft_putendl_fd("Set socket opt failed", 2);
		return (-4);
	}

	/*
	** SEND / RECEIVE MSG
	*/
	struct sockaddr_in		s_addr;
	int						i;
	t_ping					pack;

	i = 0;
	pack.head.type = 1;
	pack.head.code = 8;
	pack.id = 1;

	while (i < 16)
	{
		pack.seq[i] = i + '0';
		i++;
	}
	pack.head.chk = checksum(&pack, sizeof(pack));

	// len = sizeof(s_addr);
	// ft_bzero(buff, sizeof(buff));
	// if ( recvfrom(sock, buff, sizeof(buff), 0, &s_addr, &len) < 0 )
	// {
	// 	// ft_putendl("buff");
	// }
	s_addr.sin_family = AF_INET;

	if ( sendto(sock, &pack, sizeof(pack), 0, (struct sockaddr *)&s_addr, sizeof(s_addr)) <= 0 )
		perror("sendto");

	return 0;
}
