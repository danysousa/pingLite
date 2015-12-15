/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danysousa <danysousa@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2015/12/01 15:01:53 by danysousa         #+#    #+#             */
/*   Updated: 2015/12/01 15:03:28 by danysousa        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ping.h>

static double ft_sqrt(const double fg)
{
	double n = fg / 2.0;
	double lst = 0.0;

	while(n != lst)
	{
		lst = n;
		n = (n + fg/n) / 2.0;
	}
	return n;
}  

void		print_stat(int signo)
{
	int		percent;
	double	stddev;
	double	tmp;

	if (signo != SIGINT)
		return;
	else if (stats.received < 1)
		exit(0);
	stddev = 0;
	percent = (stats.transmitted * 100 - stats.received * 100);

	while (stats.f_time)
	{
		tmp = stats.f_time->time - (stats.avg / (double)stats.received);
		stddev += tmp * tmp;
		stats.f_time = stats.f_time->next;
	}
	stddev /= (double)stats.received;

	printf("--- %s ping statistics ---\n", stats.host);
	printf("%d packets transmitted, %d received, %d%s packet loss\n",
		stats.transmitted,
		stats.received,
		percent, "%");
	printf("round-trip min/avg/max/stddev = %lf/%lf/%lf/%lf ms\n",
		stats.min,
		stats.avg / (double)stats.received,
		stats.max,
		ft_sqrt(stddev)
		);

	exit(0);
}

unsigned short		checksum(void *b, int len)
{
	unsigned short *buf = b;
	unsigned int sum = 0;
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

void			get_timeval(struct timeval *t)
{
	gettimeofday(t, NULL);
	return ;
}

void			update_stats(double duration)
{
	stats.transmitted += 1;
	stats.received += 1;
	stats.min = (stats.min == -1 || stats.min > duration) ? duration : stats.min;
	stats.avg += duration;
	stats.max = (stats.max == -1 || stats.max < duration) ? duration : stats.max;
	if (stats.f_time == NULL)
	{
		stats.f_time = malloc(sizeof(stats.f_time));
		stats.c_time = stats.f_time;
	}
	else
	{
		stats.c_time->next = malloc(sizeof(stats.f_time));
		stats.c_time = stats.c_time->next;
	}
	stats.c_time->next = NULL;
	stats.c_time->time = duration;
}

void			display(void *buff, int count, double duration)
{
	struct iphdr	*ip = buff;
	char			src[INET_ADDRSTRLEN];

	inet_ntop( AF_INET, (void *)&ip->saddr, src, sizeof(src) );
	if (count == 1)
		printf("PING %s (%s): %ld data bytes\n", stats.host, src, PACKETSIZE-sizeof(struct icmphdr));

	printf("%d bytes from %s: icmp_seq=%d ttl=%d time=%lf ms\n",
	 ntohs(ip->tot_len) - (ip->ihl * 4),
	 src,
	 count - 1,
	 ip->ttl,
	 duration);
}
