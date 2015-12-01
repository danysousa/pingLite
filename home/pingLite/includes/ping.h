/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danysousa <danysousa@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2015/11/23 10:35:44 by danysousa         #+#    #+#             */
/*   Updated: 2015/12/01 12:22:16 by danysousa        ###   ########.fr       */
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

#define PACKETSIZE 64
struct packet
{
	struct icmphdr hdr;
	char msg[PACKETSIZE-sizeof(struct icmphdr)];
};

typedef struct		s_contact
{
	int					pid;
	int					sd;
	struct packet		pckt;
	struct sockaddr_in	*send;
	struct sockaddr_in	*recv;
}					t_contact;


#endif