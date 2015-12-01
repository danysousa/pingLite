/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danysousa <danysousa@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2015/11/23 10:35:44 by danysousa         #+#    #+#             */
/*   Updated: 2015/12/01 15:02:42 by danysousa        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PING_H
# define PING_H

# include <libft.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <netdb.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <netinet/ip_icmp.h>
# include <arpa/inet.h>
# include <sys/time.h>
# include <signal.h>
# include <strings.h>
# include <stdio.h>

# define PACKETSIZE 56

struct packet
{
	struct icmphdr hdr;
	char msg[PACKETSIZE-sizeof(struct icmphdr)];
};

typedef struct			s_contact
{
	int					pid;
	int					sd;
	struct packet		pckt;
	struct sockaddr_in	*send;
	struct sockaddr_in	*recv;
	struct timeval		send_t;
	struct timeval		recv_t;
}						t_contact;

typedef struct			s_trip
{
	double				min;
	double				avg;
	double				max;
	double				stddev;
	int					transmitted;
	int					received;
	char				*host;
}						t_trip;

t_trip					stats;

void				print_stat(int signo);
unsigned short		checksum(void *b, int len);
void				get_timeval(struct timeval *t);
void				update_stats(double duration);
void				display(void *buff, int count, double duration);

#endif