# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: mlamkadm <mlamkadm@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/11/27 22:56:49 by mlamkadm          #+#    #+#              #
#    Updated: 2024/11/27 22:56:49 by mlamkadm         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = irc-server
SRC = \
main.cpp \

DIR_SRC = src/
DIR_OBJ = obj/
DIR_INC = inc/
DIR_BIN = bin/

OBJ = $(SRC:.cpp=.o)
CXX = g++
DEP = ${OBJ:.o=.d}
obj_ext = .o

CPPFLAGS = -c -I${DIR_INC} -Wall -Wextra -O2

RM = rm -f
RMDIR = rm -rf

all: ${NAME}
    @echo "Build completed successfully."

${NAME}: ${OBJ}
    @mkdir -p ${DIR_BIN}
    ${CXX} $^ -o $@

${OBJ}: ${DIR_OBJ}%$(obj_ext)
    @mkdir -p $(dir $@)
    ${CXX} ${CPPFLAGS} $< -c -o $@
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
