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

NAME := irc-server
VERSION := nightly_build@0.7.0

DIR_SRC := src
DIR_OBJ := obj
DIR_INC := inc
DIR_BIN := bin
DIR_TEST := test
DIR_BOT := src/bot
DIR_LOG := log

SRC_FILES := HandleEvents.cpp \
             server/server.cpp \
             client/client.cpp \
             commands/cmd.cpp \
			 utils/Utils.cpp \
             main.cpp \
             Validation.cpp \
				Channel.cpp \

SRC := $(addprefix $(DIR_SRC)/, $(SRC_FILES)) #too good not to be forbidden (check 42-docs)
OBJ := $(SRC:$(DIR_SRC)/%.cpp=$(DIR_OBJ)/%.o)
DEP := $(OBJ:.o=.d)

CXX := c++
CXXFLAGS := -Wall -Wextra -Werror -std=c++98
CPPFLAGS := -MMD -MP -I$(DIR_INC)
LDFLAGS := -L/usr/local/lib


#-DEBUG-##########################################################
ifeq ($(DEBUG), 1)
    CXXFLAGS += -g3 -DDEBUG
else
    CXXFLAGS += -O2
endif

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
    LDLIBS += -lpthread
endif

#-ENV-############################################################
PORT ?= 22200
PASSWORD ?= Alilepro135!
ARGS := $(PORT) $(PASSWORD)
##################################################################

RM := rm -f
RMDIR := rm -rf
MKDIR := mkdir -p
ECHO := echo

.PHONY: all clean fclean re debug release test run bot help

all: $(NAME)

$(NAME): $(OBJ) | $(DIR_BIN)
	@$(CXX) $(OBJ) $(LDFLAGS) $(LDLIBS) -o $(DIR_BIN)/$@

$(DIR_OBJ)/%.o: $(DIR_SRC)/%.cpp | $(DIR_OBJ)
	@$(MKDIR) $(dir $@)
	@$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

$(DIR_BIN) $(DIR_OBJ) $(DIR_LOG):
	@$(MKDIR) $@

-include $(DEP)

clean:
	@$(RMDIR) $(DIR_OBJ)
	@$(RMDIR) $(DIR_LOG)

fclean: clean
	@$(RMDIR) $(DIR_BIN)
	@$(MAKE) -C $(DIR_BOT) fclean 2>/dev/null || true

re: fclean all

debug: CXXFLAGS += -g3 -DDEBUG
debug: all

release: CXXFLAGS += -O2 -DNDEBUG
release: all

test: debug | $(DIR_LOG)
	@./$(DIR_TEST)/main.sh -v -d -t 1 2>&1 | tee $(DIR_LOG)/test.log

run: re
	@./$(DIR_BIN)/$(NAME) $(ARGS)

bot:
	@$(MAKE) -C $(DIR_BOT)

help:
	@$(ECHO) "Available targets:"
	@$(ECHO) "  make all      - Build the IRC server (default)"
	@$(ECHO) "  make clean    - Remove object files"
	@$(ECHO) "  make fclean   - Remove object files and binary"
	@$(ECHO) "  make re       - Rebuild everything"
	@$(ECHO) "  make debug    - Build with debug symbols"
	@$(ECHO) "  make release  - Build with optimizations"
	@$(ECHO) "  make test     - Run tests"
	@$(ECHO) "  make run      - Build and run server"
	@$(ECHO) "  make bot      - Build the bot component"
	@$(ECHO) "  make help     - Show this help message"
	@$(ECHO) "Configuration:"
	@$(ECHO) "  PORT=<port>         - Set server port (default: $(PORT))"
	@$(ECHO) "  PASSWORD=<pass>     - Set server password"
	@$(ECHO) "  DEBUG=1             - Enable debug build"

version:
	@$(ECHO) "$(NAME) version $(VERSION)"
