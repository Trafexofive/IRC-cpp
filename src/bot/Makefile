# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: mlamkadm <mlamkadm@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/09/19 20:24:35 by mlamkadm          #+#    #+#              #
#    Updated: 2024/09/19 20:24:35 by mlamkadm         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = bin/bot

SRC_DIR = src
HEADER = inc/bot.hpp 
OBJ_DIR = obj

SRC = main.cpp bot.cpp parse.cpp connection.cpp display.cpp message.cpp channel.cpp core.cpp Reminder.cpp commands.cpp Uptime.cpp utils.cpp display.cpp Environment.cpp

OBJ = $(SRC:%.cpp=$(OBJ_DIR)/%.o)

FLAGS = -Wall -Wextra -Werror

CPP98 = -std=c++98

ARGS = -C conf/config.txt


all: $(NAME)

build: all clean

run: all clean
	./$(NAME) $(ARGS)

$(NAME): $(OBJ)
	@mkdir -p $(dir $(NAME))
	c++ $(FLAGS) $(OBJ) -o $(NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	c++ $(FLAGS) -c $< -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all build run clean fclean re

