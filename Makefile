# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: marschul <marschul@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/04/17 16:12:42 by marschul          #+#    #+#              #
#    Updated: 2024/04/17 16:28:48 by marschul         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME	:= ircserv

CC 		:= c++
CFLAGS 	:= -Wall -Wextra -Werror -std=c++98 
DFLAGS 	:= -Wall -Wextra -g3 -fsanitize=address
DNAME 	:= ircserv_debug
HEADERS = -I./includes

SRCDIR		:= srcs
OBJDIR		:= objs

SRCS 	:= $(SRCDIR)/main.cpp

OBJS	:= $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SRCS))

all: $(NAME)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CC) $(DFLAGS) $(HEADERS) -c $< -o $@

$(NAME): $(OBJS)
	$(CC) $(DFLAGS) $(HEADERS) $(OBJS) -o $(NAME)

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME) $(DNAME)

re: fclean all

.PHONY: all, clean, fclean, re, debug
