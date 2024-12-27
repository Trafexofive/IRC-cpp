# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    test.sh                                            :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: mlamkadm <mlamkadm@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/12/27 09:13:39 by mlamkadm          #+#    #+#              #
#    Updated: 2024/12/27 09:13:39 by mlamkadm         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

#!/bin/bash

# Configuration
PORT=12600
PASSWORD="Alilepro135!"
SERVER_BINARY="./bin/irc-server"

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

# Print header
echo -e "${BLUE}IRC Server Test Script"
echo "Current Date and Time (UTC): $(date -u '+%Y-%m-%d %H:%M:%S')"
echo "Testing server at localhost:$PORT"
echo -e "Using password: $PASSWORD${NC}"
echo "----------------------------------------"

# Check if server binary exists
if [ ! -f "$SERVER_BINARY" ]; then
    echo -e "${RED}Error: Server binary not found at $SERVER_BINARY${NC}"
    exit 1
fi

# Start server in background
echo -e "\n${YELLOW}Starting IRC server...${NC}"
$SERVER_BINARY $PORT $PASSWORD > /dev/null 2>&1 &
SERVER_PID=$!

# Wait for server to start
sleep 2

# Test connection using netcat
echo -e "\n${YELLOW}Testing connection...${NC}"
if nc -z localhost $PORT 2>/dev/null; then
    echo -e "${GREEN}✓ Server is running${NC}"
else
    echo -e "${RED}✗ Server failed to start${NC}"
    kill $SERVER_PID 2>/dev/null
    exit 1
fi

# Run tests
echo -e "\n${YELLOW}Running tests...${NC}"

# Test 1: Authentication
echo -e "\n${YELLOW}Test 1: Authentication${NC}"
{
    echo "PASS $PASSWORD"
    sleep 1
    echo "NICK testuser"
    sleep 1
    echo "USER testuser 0 * :Test User"
    sleep 1
} | nc localhost $PORT

# Test 2: Channel operations
echo -e "\n${YELLOW}Test 2: Channel Operations${NC}"
{
    echo "PASS $PASSWORD"
    sleep 1
    echo "NICK testuser2"
    sleep 1
    echo "USER testuser2 0 * :Test User 2"
    sleep 1
    echo "JOIN #testchannel"
    sleep 1
    echo "PRIVMSG #testchannel :Hello, World!"
    sleep 1
} | nc localhost $PORT

# Cleanup
echo -e "\n${YELLOW}Stopping server...${NC}"
kill $SERVER_PID 2>/dev/null
wait $SERVER_PID 2>/dev/null
echo -e "${GREEN}✓ Tests completed${NC}"

