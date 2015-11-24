/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danysousa <danysousa@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2015/11/23 10:35:44 by danysousa         #+#    #+#             */
/*   Updated: 2015/11/23 16:45:16 by danysousa        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PING_H
# define PING_H

# include <libft.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <netdb.h>
# include <sys/socket.h>

typedef struct icmp
{
	//! ICMP message type.
	uint8_t		type;
	//! ICMP operation code.
	uint8_t		code;
	//! ICMP checksum.
	uint16_t	chk;
}				t_icmp;

typedef struct s_ping
{
	t_icmp		head;
	uint16_t	id;
	char		seq[16];
}				t_ping;


#endif