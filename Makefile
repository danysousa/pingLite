# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: danysousa <danysousa@student.42.fr>        +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2014/02/26 13:13:47 by rbenjami          #+#    #+#              #
#    Updated: 2015/11/23 10:32:48 by danysousa        ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

include			libft/Makefile.sources

export	CC		=	cc

export	INC		=	-I $(PWD)/libft

NAME			=	ping

CFLAGS			=	-Wall -Wextra -Werror -g

INC				+=	-I includes

INC_DIR			=	includes/

INC_FILES		=

INC_SRC			=	$(addprefix $(INC_DIR), $(INC_FILES))

SRC_DIR			=	srcs/

FILES			=	ping.c

SRC				=	$(addprefix $(SRC_DIR), $(FILES))

OBJ				=	$(SRC:.c=.o)

LIB				=	-L./ -lft

OBJ_LIB			=	$(libft/SRC_LIB:.c=.o)

HEAD_LIB		=	libft/libft.h

all:			libft.a $(NAME)

libft.a:		libft/$(OBJ_LIB) $(HEAD_LIB)
	@make -C libft

$(NAME):		$(OBJ)
	@$(CC) $(CFLAGS) -o $(NAME) $(OBJ) $(LIB)
	@echo ""
	@echo "\033[33m"compilation of $(NAME) : "\033[32m"Success"\033[0m"

$(OBJ):			$(INC_SRC) libft.a

%.o:			%.c $(INC_SRC)
	@echo -n .
	@$(CC) $(CFLAGS) -c $< -o $@ $(INC)

clean:
	@rm -f $(OBJ)
	@echo "\033[31m"Objects of $(NAME): deleted"\033[0m"

fclean:			clean
	@rm -f $(NAME)
	@echo "\033[31m"$(NAME) : deleted"\033[0m"
	@make fclean -C libft

re:				fclean all

.PHONY:			all clean fclean re
