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


# Project Information
NAME := irc-server
VERSION := nightly_build@0.7.0
AUTHOR := $(shell whoami)
DATE := $(shell date '+%Y-%m-%d %H:%M:%S')

# Directory Structure
DIR_SRC := src
DIR_OBJ := obj
DIR_INC := inc
DIR_BIN := bin
DIR_TEST := test
DIR_BOT := src/bot
DIR_LOG := log
DIR_BACKUP := backup
DIR_DEPS := .deps

# Test File
TEST-FILE := $(DIR_TEST)/test-cases/ping.test

# Source Files
SRC_FILES := HandleEvents.cpp \
    server/server.cpp \
    client/client.cpp \
    commands/cmd.cpp \
    utils/Utils.cpp \
    main.cpp \
    Validation.cpp \
    Channel.cpp

# File Management
SRC := $(addprefix $(DIR_SRC)/, $(SRC_FILES))
OBJ := $(SRC:$(DIR_SRC)/%.cpp=$(DIR_OBJ)/%.o)
DEP := $(OBJ:%.o=$(DIR_DEPS)/%.d)

# Compiler and Flags
CXX := c++
CXXFLAGS := -Wall -Wextra -Werror -std=c++98
CPPFLAGS := -MMD -MP -I$(DIR_INC)
LDFLAGS := -L/usr/local/lib

# Debug Configuration
ifeq ($(DEBUG), 1)
    CXXFLAGS += -g3 -DDEBUG
    BUILD_TYPE := Debug
else
    CXXFLAGS += -O2 -DNDEBUG
    BUILD_TYPE := Release
endif

# Platform-specific Configuration
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
    LDLIBS += -lpthread
endif

# Environment Variables
PORT ?= 22200
PASSWORD ?= Alilepro135!
ARGS := $(PORT) $(PASSWORD)

# System Commands
RM := rm -f
RMDIR := rm -rf
MKDIR := mkdir -p
ECHO := echo

.PHONY: all clean fclean re debug release test run bot help version banner

all: banner $(NAME)

banner:
	@printf "\n%s\n" "===================================="
	@printf "%s\n" "  Building $(NAME) - $(VERSION)"
	@printf "%s\n" "  Build Type: $(BUILD_TYPE)"
	@printf "%s\n" "  Author: $(AUTHOR)"
	@printf "%s\n\n" "===================================="

$(NAME): $(OBJ) | $(DIR_BIN)
	@printf "Linking $(NAME)...\n"
	@$(CXX) $(OBJ) $(LDFLAGS) $(LDLIBS) -o $(DIR_BIN)/$@
	@printf "Build complete!\n"

$(DIR_OBJ)/%.o: $(DIR_SRC)/%.cpp | $(DIR_OBJ) $(DIR_DEPS)
	@$(MKDIR) $(dir $@)
	@$(MKDIR) $(dir $(DIR_DEPS)/$*)
	@printf "Compiling $<...\n"
	@$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@
	@mv $(DIR_OBJ)/$*.d $(DIR_DEPS)/$*.d

$(DIR_BIN) $(DIR_OBJ) $(DIR_LOG) $(DIR_DEPS):
	@$(MKDIR) $@

-include $(DEP)

kill_port:
	@if lsof -i :$(PORT) > /dev/null 2>&1; then \
        echo "Killing process on port $(PORT)"; \
        lsof -ti :$(PORT) | xargs kill -9; \
    else \
        echo "No process running on port $(PORT)"; \
    fi

clean: kill_port
	@printf "Cleaning object files...\n"
	@$(RMDIR) $(DIR_OBJ)
	@$(RMDIR) $(DIR_DEPS)
	@$(RMDIR) $(DIR_LOG)

fclean: clean
	@printf "Cleaning everything...\n"
	@$(RMDIR) $(DIR_BIN)
	@$(MAKE) -C $(DIR_BOT) fclean 2>/dev/null || true

re: fclean all

debug: 
	@$(MAKE) DEBUG=1 all

release:
	@$(MAKE) all

test: re debug | $(DIR_LOG)
	@printf "Running tests...\n"
	@./$(DIR_TEST)/main.sh -f $(TEST-FILE) -d -v -t 1 2>&1 | tee $(DIR_LOG)/test.log

run: clean all
	@printf "Running $(NAME)...\n"
	@./$(DIR_BIN)/$(NAME) $(ARGS)

client: 
	@printf "Running client...\n"
	python3 ./test/TUI-client.py -p 22200 --password Alilepro135!

bot: all
	@printf "Building bot...\n"
	@$(MAKE) -C $(DIR_BOT)

help:
	@printf "\nAvailable targets:\n"
	@printf "  make all      - Build the IRC server (default)\n"
	@printf "  make clean    - Remove object files\n"
	@printf "  make fclean   - Remove object files and binary\n"
	@printf "  make re       - Rebuild everything\n"
	@printf "  make debug    - Build with debug symbols\n"
	@printf "  make release  - Build with optimizations\n"
	@printf "  make test     - Run tests\n"
	@printf "  make run      - Build and run server\n"
	@printf "  make bot      - Build the bot component\n"
	@printf "  make help     - Show this help message\n"
	@printf "\nConfiguration:\n"
	@printf "  PORT=<port>         - Set server port (default: $(PORT))\n"
	@printf "  PASSWORD=<pass>     - Set server password\n"
	@printf "  DEBUG=1             - Enable debug build\n\n"

version:
	@printf "$(NAME) version $(VERSION)\n"
	@printf "Built on: $(DATE)\n"
	@printf "By: $(AUTHOR)\n"
