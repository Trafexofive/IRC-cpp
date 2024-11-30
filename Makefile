# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: mlamkadm <mlamkadm@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/11/30 18:07:15 by mlamkadm          #+#    #+#              #
#    Updated: 2024/11/30 18:07:15 by mlamkadm         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #


NAME = irc-server
SRC = \
    core_server.cpp \
    events_handling.cpp \
    server.cpp \

DIR_SRC = src/
DIR_OBJ = obj/
DIR_INC = inc/
DIR_BIN = bin/

OBJ = ${SRC:%.cpp=${DIR_OBJ}%.o}
CXX = c++
DEP = ${OBJ:%.o=%.d}
CPPFLAGS = -c -I ${DIR_INC}
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
	./test.sh



