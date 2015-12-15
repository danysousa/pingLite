/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danysousa <danysousa@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2015/11/23 10:33:21 by danysousa         #+#    #+#             */
/*   Updated: 2015/12/01 15:01:48 by danysousa        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ping.h>

int		init_sock()
{
	int			sd;
	const int	val = 64;

	sd = socket(PF_INET, SOCK_RAW, IPPROTO_ICMP);
	if ( sd < 0 )
	{
		ft_putendl_fd("Error with init socket", 2);
		return (-1);
	}
	if ( setsockopt(sd, SOL_IP, IP_TTL, &val, sizeof(val)) != 0)
	{
		ft_putendl_fd("Error with setsockopt", 2);
		return (-2);
	}

	return (sd);
}

struct sockaddr_in		*get_sock_info()
{
	struct addrinfo			*addrinfo;
	struct sockaddr_in		*addr_ping;
	char					*hostname;

	if (ft_strcmp(stats.host, "localhost") == 0)
		hostname = ft_strdup("127.0.0.1");
	else
		hostname = stats.host;
	addr_ping = malloc(sizeof(struct sockaddr_in));
	ft_bzero(addr_ping, sizeof(addr_ping));
	if (getaddrinfo(hostname, NULL, NULL, &addrinfo) != 0)
	{
		ft_putendl_fd("Can't resolve this host", 2);
		return (NULL);
	}
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

	/*
	** INIT PACKET
	*/
	ft_bzero(&contact->pckt, sizeof(contact->pckt));
	contact->pckt.hdr.type = ICMP_ECHO;
	contact->pckt.hdr.un.echo.id = contact->pid;
	for ( i = 0; i < sizeof(contact->pckt.msg) - 1; i++ )
		contact->pckt.msg[i] = i + '0';
	contact->pckt.msg[i] = 0;
	contact->pckt.hdr.un.echo.sequence = cnt;
	contact->pckt.hdr.checksum = checksum(&contact->pckt, sizeof(contact->pckt));

	/*
	** SEND & RECEIVE
	*/
	if ( sendto(contact->sd, &contact->pckt, sizeof(contact->pckt), 0, (struct sockaddr*)contact->send, sizeof(*contact->send)) <= 0 )
	{
		ft_putendl_fd("Error with send packet", 2);
		return (-3);
	}
	get_timeval(&contact->send_t);
	if ( recvfrom(contact->sd, &contact->pckt, sizeof(contact->pckt), 0, (struct sockaddr*)contact->recv, &len) < 0 )
	{
		ft_putendl_fd("Error with recv packet", 2);
		return (-4);
	}
	get_timeval(&contact->recv_t);
	return (0);
}

int						ping(int pid)
{
	t_contact				contact;
	int						cnt;
	int						err;
	double					duration;

	cnt = 1;
	contact.send = get_sock_info();
	if (contact.send == NULL)
		return (-5);
	contact.recv = malloc(sizeof(struct sockaddr_in));
	contact.sd = init_sock();
	contact.pid = pid;

	if (contact.sd < 0)
		return (contact.sd);

	while (0x2A)
	{
		err = loop(&contact, cnt);
		if (err != 0)
			return (err);
		duration = (double)(contact.recv_t.tv_sec * 1000) + (double)((double)contact.recv_t.tv_usec / 1000);
		duration -= (double)(contact.send_t.tv_sec * 1000) + (double)((double)contact.send_t.tv_usec / 1000);
		update_stats(duration);
		display(&contact.pckt, cnt, duration);
		cnt++;
		sleep(1);
	}

	return 1;
}

void		ft_option(char *argv[])
{
	if (!ft_strcmp(argv[1], "-v"))
		return ;
	else
	{
		ft_putendl("Send ICMP ECHO_REQUEST packets to network hosts.\n\n\
Options valid for all request types:\n\
-h\t\tthat's it !\n\
-v\t\tverbose\n\
Mandatory or optional arguments to long options are also mandatory or optional for any corresponding short options.\n\
Options marked with (root only) are available only to superuser.\n\n\
Report bugs to ... no report bugs !:)");
		exit(0);
	}
}

int			main(int argc, char *argv[])
{
	int		pid;

	pid = getpid();

	if (argc == 3 || (argc == 2 && argv[1][0] == '-'))
		ft_option(argv);

	if (argc < 2 || (argc > 2 && argv[1][0] != '-') || (argc == 2 && argv[1][0] == '-'))
	{
		ft_putendl_fd("Usage: ./ping [-v or -h][hostname/ip]", 2);
		return (-1);
	}

	stats.min = -1;
	stats.avg = 0;
	stats.max = -1;
	stats.stddev = 0;
	stats.transmitted = 0;
	stats.received = 0;
	stats.host = (argc == 3) ? argv[2] : argv[1];
	stats.f_time = NULL;
	stats.c_time = NULL;
	signal(SIGINT, print_stat);
	return ping(pid);
}

