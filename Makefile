# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: mlamkadm <mlamkadm@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/12/07 20:37:57 by mlamkadm          #+#    #+#              #
#    Updated: 2024/12/07 20:37:57 by mlamkadm         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = irc-server
SRC = \
    HandleEvents.cpp \
    server.cpp \

DIR_SRC = src/
DIR_OBJ = obj/
DIR_INC = inc/
DIR_BIN = bin

EVENTS = -L/usr/local/lib -I/usr/local/include

OBJ = ${SRC:%.cpp=${DIR_OBJ}%.o}
CXX = c++
DEP = ${OBJ:%.o=%.d}
CPPFLAGS = -Wall -Wextra -Werror -c ${EVENTS} -I ${DIR_INC}
RM = rm -f
RMDIR = rm -rf

ARGS = alilepro 3344

all: ${NAME}

${NAME}: ${OBJ}
	@mkdir -p ${DIR_BIN}
	${CXX} $^ -o ${DIR_BIN}/$@

${OBJ}: ${DIR_OBJ}%.o: ${DIR_SRC}%.cpp
	@mkdir -p ${@D}
	${CXX} ${CPPFLAGS} $< -o $@

-include ${DEP}

clean:
	${RMDIR} ${DIR_OBJ}
	${RM} ${DIR_BIN}/${NAME}

fclean: clean
	${RM} ${DIR_BIN}/${NAME}

re: fclean all

run:
	@make clean
	@./${DIR_BIN}/${NAME} $(ARGS)

test:
	# ./test.sh

.PHONY: all clean fclean re

