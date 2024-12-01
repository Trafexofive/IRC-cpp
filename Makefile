SRCS = src/core_server.cpp src/events_handling.cpp src/server.cpp

# Directory for object files
OBJ_DIR = obj

# Create object file paths based on source files
OBJS = $(SRCS:src/%.cpp=$(OBJ_DIR)/%.o)

NAME = weusearch

HEADER = includes/server.hpp

CC = c++

CFLAGS = -Wall -Werror -std=c++98

RM = rm -rf

# Default target: Build the executable
all: $(NAME)

# Rule to build the executable
$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

# Rule to compile .cpp to .o inside the obj directory
$(OBJ_DIR)/%.o: src/%.cpp $(HEADER)
	@mkdir -p $(dir $@)  # Create the necessary subdirectories under obj
	@echo "Compiling $<"
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up object files in the obj directory
clean:
	$(RM) $(OBJ_DIR)

# Clean up both object files and the binary
fclean: clean
	$(RM) $(NAME)

# Rebuild everything
re: fclean all

# Declare phony targets
.PHONY: all re fclean clean
