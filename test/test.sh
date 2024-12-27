# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    test.sh                                            :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: mlamkadm <mlamkadm@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/12/27 10:32:22 by mlamkadm          #+#    #+#              #
#    Updated: 2024/12/27 10:32:22 by mlamkadm         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

#!/bin/bash

# Configuration
PORT=22200
PASSWORD="Alilepro135!"
SERVER_BINARY="./bin/irc-server"
TIMEOUT=2

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

function run_test() {
    local test_name=$1
    local expected=$2
    local commands=$3
    
    echo -e "\n${BOLD}Running: ${test_name}${NC}"
    print_separator
    
    # Create a temporary file for the actual output
    local output_file=$(mktemp)
    
    # Run the test with timeout
    echo -e "${GRAY}Sending:${NC}\n$commands"
    timeout $TIMEOUT bash -c "echo -e \"$commands\" | nc localhost $PORT" > "$output_file" 2>&1
    local exit_code=$?
    
    # Read the actual output
    local received=$(cat "$output_file")
    rm -f "$output_file"
    
    print_separator
    
    # Check for timeout or failure
    if [ $exit_code -eq 124 ]; then
        echo -e "${KO} Test timed out after ${TIMEOUT} seconds"
        echo -e "${YELLOW}Expected:${NC} $expected"
        echo -e "${RED}Received:${NC} No response (hanging)"
        return 1
    elif [ $exit_code -ne 0 ]; then
        echo -e "${KO} Test failed with exit code $exit_code"
        echo -e "${YELLOW}Expected:${NC} $expected"
        echo -e "${RED}Received:${NC} $received"
        return 1
    fi
    
    # Check if output matches expected
    if [[ "$received" == *"$expected"* ]]; then
        echo -e "${OK} Test passed"
        echo -e "${YELLOW}Expected:${NC} $expected"
        echo -e "${GREEN}Received:${NC} $received"
        return 0
    else
        echo -e "${KO} Test failed"
        echo -e "${YELLOW}Expected:${NC} $expected"
        echo -e "${RED}Received:${NC} ${received:-<no output>}"
        return 1
    fi
}

function cleanup() {
    if [ -n "$SERVER_PID" ]; then
        kill -9 $SERVER_PID 2>/dev/null
        wait $SERVER_PID 2>/dev/null
    fi
}

trap cleanup EXIT

# Print header
echo -e "${BLUE}${BOLD}IRC Server Test Suite${NC}"
echo "Current Date and Time (UTC): $(date -u '+%Y-%m-%d %H:%M:%S')"
echo "Current User's Login: Trafexofive"
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
sleep 1

# Verify server is running
if ! ps -p $SERVER_PID > /dev/null || ! nc -z localhost $PORT 2>/dev/null; then
    echo -e "${RED}Error: Server failed to start or is not listening on port $PORT${NC}"
    cleanup
    exit 1
fi

echo -e "${GREEN}Server started successfully${NC}"
print_separator

# Run tests
# Test 1: Authentication
run_test "Authentication Test" \
         "001" \
         "PASS $PASSWORD\r\nNICK testuser\r\nUSER testuser 0 * :Test User\r\n"

# Test 2: Invalid Password
run_test "Invalid Password Test" \
         "464" \
         "PASS wrongpassword\r\nNICK testuser\r\nUSER testuser 0 * :Test User\r\n"

# Test 3: Channel Join
run_test "Channel Join Test" \
         "JOIN" \
         "PASS $PASSWORD\r\nNICK testuser2\r\nUSER testuser2 0 * :Test User\r\nJOIN #testchannel\r\n"

# Test 4: Empty Password
run_test "Empty Password Test" \
         "464" \
         "PASS \r\nNICK testuser\r\nUSER testuser 0 * :Test User\r\n"

# Test 5: Message to Channel
run_test "Channel Message Test" \
         "PRIVMSG" \
         "PASS $PASSWORD\r\nNICK testuser3\r\nUSER testuser3 0 * :Test User\r\nJOIN #testchannel\r\nPRIVMSG #testchannel :Hello\r\n"

echo -e "\n${YELLOW}Stopping server...${NC}"
cleanup
print_separator
echo -e "${GREEN}Tests completed${NC}"
