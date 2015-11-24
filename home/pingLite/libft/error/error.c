/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danysousa <danysousa@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2014/05/06 14:46:33 by dsousa            #+#    #+#             */
/*   Updated: 2015/10/17 13:58:08 by danysousa        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdarg.h>
#include <stdio.h>
#include "libft.h"

int		error(const char *msg, ...)
{
	va_list		ap;

	va_start(ap, msg);
	while (*msg)
	{
		if (*msg == '%')
		{
			msg++;
			if (*msg == '%')
				ft_putchar_fd('%', 2);
			else if (*msg == 'c')
				ft_putchar_fd(va_arg(ap, int), 2);
			else if (*msg == 's')
				ft_putstr_fd(va_arg(ap, char*), 2);
			else if (*msg == 'd')
				ft_putnbr_base_fd(va_arg(ap, int), 10, 2);
		}
		else
			ft_putchar_fd(*msg, 2);
		msg++;
	}
	va_end(ap);
	return (-42);
}
