# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    test.sh                                            :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: mlamkadm <mlamkadm@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/12/27 11:16:27 by mlamkadm          #+#    #+#              #
#    Updated: 2024/12/27 11:16:27 by mlamkadm         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

#!/bin/bash

# Configuration
PORT=22200
PASSWORD="Alilepro135!"
SERVER_BINARY="./bin/irc-server"
TIMEOUT=5  # Increased timeout

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
GRAY='\033[0;90m'
NC='\033[0m'
BOLD='\033[1m'

# Test status indicators
OK="${GREEN}[OK]${NC}"
KO="${RED}[KO]${NC}"

function print_separator() {
    echo -e "${GRAY}----------------------------------------${NC}"
}

function send_and_receive() {
    local commands=$1
    local expected=$2
    local response=""
    
    # Create a temporary file for the response
    local tmpfile=$(mktemp)
    
    # Use netcat with a timeout to send commands and receive response
    (echo -e "$commands"; sleep 1) | timeout $TIMEOUT nc localhost $PORT > "$tmpfile"
    
    # Read the response
    response=$(<"$tmpfile")
    rm -f "$tmpfile"
    
    # Debug output
    echo -e "${GRAY}Debug: Raw response:${NC}"
    echo "$response" | xxd
    
    # Check if response contains expected string
    if [[ "$response" == *"$expected"* ]]; then
        return 0
    else
        return 1
    fi
}

function run_test() {
    local test_name=$1
    local expected=$2
    local commands=$3
    
    echo -e "\n${BOLD}Running: ${test_name}${NC}"
    print_separator
    echo -e "${GRAY}Sending:${NC}\n$commands"
    print_separator

    # Add debug output for what we're expecting
    echo -e "${GRAY}Debug: Expecting to find:${NC} '$expected'"
    
    if send_and_receive "$commands" "$expected"; then
        echo -e "${OK} Test passed"
        return 0
    else
        echo -e "${KO} Test failed"
        echo -e "${YELLOW}Expected:${NC} $expected"
        echo -e "${RED}Actual response did not contain expected string${NC}"
        return 1
    fi
}

function cleanup() {
    if [ -n "$SERVER_PID" ]; then
        kill -9 $SERVER_PID 2>/dev/null
        wait $SERVER_PID 2>/dev/null
    fi
}

trap cleanup EXIT INT TERM

# Print header
echo -e "${BLUE}${BOLD}IRC Server Test Suite${NC}"
echo "Current Date and Time (UTC): $(date -u '+%Y-%m-%d %H:%M:%S')"
echo "Current User's Login: $(whoami)"
echo "Testing server at localhost:$PORT"
print_separator

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

# Verify server is running
if ! ps -p $SERVER_PID > /dev/null || ! nc -z localhost $PORT 2>/dev/null; then
    echo -e "${RED}Error: Server failed to start or is not listening on port $PORT${NC}"
    cleanup
    exit 1
fi

echo -e "${GREEN}Server started successfully${NC}"
print_separator

# Run tests with modified expected responses
run_test "Authentication Test" \
         "Welcome" \
         "PASS $PASSWORD\r\nNICK testuser\r\nUSER testuser 0 * :Test User\r\n"

sleep 1

run_test "Invalid Password Test" \
         "incorrect" \
         "PASS wrongpassword\r\nNICK testuser\r\nUSER testuser 0 * :Test User\r\n"

sleep 1

run_test "Channel Join Test" \
         "JOIN" \
         "PASS $PASSWORD\r\nNICK testuser2\r\nUSER testuser2 0 * :Test User\r\nJOIN #testchannel\r\n"

sleep 1

run_test "Empty Password Test" \
         "parameters" \
         "PASS \r\nNICK testuser\r\nUSER testuser 0 * :Test User\r\n"

sleep 1

run_test "Channel Message Test" \
         "PRIVMSG" \
         "PASS $PASSWORD\r\nNICK testuser3\r\nUSER testuser3 0 * :Test User\r\nJOIN #testchannel\r\nPRIVMSG #testchannel :Hello\r\n"

echo -e "\n${YELLOW}Stopping server...${NC}"
cleanup
print_separator
echo -e "${GREEN}Tests completed${NC}"
