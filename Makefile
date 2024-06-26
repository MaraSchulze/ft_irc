# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: marschul <marschul@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/04/17 16:12:42 by marschul          #+#    #+#              #
#    Updated: 2024/05/21 17:56:49 by marschul         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME	:= ircserv

CC 		:= c++
CFLAGS 	:= -Wall -Wextra -Werror -std=c++98 
DFLAGS 	:= -Wall -Wextra -g3 -fsanitize=address
HEADERS = -I./includes

SRCDIR		:= srcs
OBJDIR		:= objs

SRCS 	:= main.cpp IrcApplicationLayerPublic.cpp IrcApplicationLayerCommands.cpp \
			IrcApplicationLayerSendFunctions.cpp IrcApplicationLayerHelperFunctions.cpp\
			User.cpp Channel.cpp helperFunctions.cpp Mode.cpp SendQueue.cpp Server.cpp

SRCS_WITH_DIRPREFIX	:= $(addprefix $(SRCDIR)/, $(SRCS))

OBJS	:= $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SRCS_WITH_DIRPREFIX))

all: $(NAME)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CC) $(CFLAGS) $(HEADERS) -c $< -o $@

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(HEADERS) $(OBJS) -o $(NAME)

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME) $(DNAME)

re: fclean all

.PHONY: all, clean, fclean, re
