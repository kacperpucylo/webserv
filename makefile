# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: kpucylo <kpucylo@student.42wolfsburg.de    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2021/05/21 14:41:30 by mchatzip          #+#    #+#              #
#    Updated: 2022/06/30 11:57:49 by kpucylo          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

CC		= g++

CFLAGS	= -Wall -Wextra -Werror -std=c++98 -pedantic

RM		= rm -f

NAME	= webserv

SRC		= refactored_temp.cpp

OBJ		= ${SRC:.c=.o}

all:		$(NAME)

$(NAME):	$(OBJ)
			$(CC) -g $(CFLAGS) -o $(NAME) $(OBJ)

clean:		
			$(RM) *.o

fclean:		clean
			$(RM) $(NAME)
			rm -r webserv.dSYM

re:			fclean all

.PHONY: all clean fclean re