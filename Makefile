# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: mboutuil <mboutuil@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/11/30 18:07:15 by mlamkadm          #+#    #+#              #
#    Updated: 2024/12/02 10:03:50 by mboutuil         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #


NAME = irc-server
SRC = \
    HandleEvents.cpp \
    server.cpp \

DIR_SRC = src/
DIR_OBJ = obj/
DIR_INC = inc/
DIR_BIN = bin/


EVENTS = -lkqueue -L/usr/local/lib -I/usr/local/include

OBJ = ${SRC:%.cpp=${DIR_OBJ}%.o}
CXX = c++
DEP = ${OBJ:%.o=%.d}
CPPFLAGS = -Wall -Wextra -Werror -c ${EVENTS}  -I ${DIR_INC}
RM = rm -f
RMDIR = rm -rf

all: ${NAME}

${NAME}: ${OBJ}
	${CXX} $^ -o $@

${OBJ}: ${DIR_OBJ}%.o: ${DIR_SRC}%.cpp
	@mkdir -p ${@D}
	${CXX} ${CPPFLAGS} $< -o $@

-include ${DEP}

clean:
	${RMDIR} ${DIR_OBJ}
	${RM} ${NAME}

fclean: clean
	${RM} ${NAME}

re: fclean all

.PHONY: all clean fclean re

run:
	@make fclean
	@./${DIR_BIN}/${NAME}

test:
	# ./test.sh



