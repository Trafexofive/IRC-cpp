# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    test.sh                                            :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: mlamkadm <mlamkadm@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/12/27 10:23:07 by mlamkadm          #+#    #+#              #
#    Updated: 2024/12/27 10:23:07 by mlamkadm         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

#!/bin/bash

# Configuration
PORT=22200  # Updated to match your server's port
PASSWORD="Alilepro135!"
SERVER_BINARY="./bin/irc-server"
TIMEOUT=2

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'
BOLD='\033[1m'

# Test status indicators
OK="${GREEN}[OK]${NC}"
KO="${RED}[KO]${NC}"

# Function to run test with timeout and capture output
function run_test() {
    local test_name=$1
    local expected=$2
    local commands=$3
    
    # Create a temporary file for the actual output
    local output_file=$(mktemp)
    
    # Run the test with timeout
    timeout $TIMEOUT bash -c "echo -e \"$commands\" | nc localhost $PORT" > "$output_file" 2>&1
    local exit_code=$?
    
    # Read the actual output
    local actual=$(cat "$output_file")
    rm -f "$output_file"
    
    # Check for timeout or failure
    if [ $exit_code -eq 124 ]; then
        printf "%-40s %s\n" "$test_name" "$KO"
        echo -e "${RED}Error: Test timed out after ${TIMEOUT} seconds${NC}"
        echo -e "${RED}Expected: $expected${NC}"
        echo -e "${RED}Got: No response (hanging)${NC}"
        return 1
    elif [ $exit_code -ne 0 ]; then
        printf "%-40s %s\n" "$test_name" "$KO"
        echo -e "${RED}Error: Test failed with exit code $exit_code${NC}"
        echo -e "${RED}Expected: $expected${NC}"
        echo -e "${RED}Got: $actual${NC}"
        return 1
    fi
    
    # Check if output matches expected
    if [[ "$actual" == *"$expected"* ]]; then
        printf "%-40s %s\n" "$test_name" "$OK"
        return 0
    else
        printf "%-40s %s\n" "$test_name" "$KO"
        echo -e "${RED}Expected: $expected${NC}"
        echo -e "${RED}Got: ${actual:0:50}...${NC}"
        return 1
    fi
}

# Cleanup function
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
echo "Testing server at localhost:$PORT"
echo -e "----------------------------------------\n"

# Check if server binary exists
if [ ! -f "$SERVER_BINARY" ]; then
    echo -e "${RED}Error: Server binary not found at $SERVER_BINARY${NC}"
    exit 1
fi

# Start server in background
echo -e "${YELLOW}Starting IRC server...${NC}"
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

echo -e "${GREEN}Server started successfully${NC}\n"

# Run tests
echo -e "${YELLOW}Running Tests...${NC}\n"

# Test 1: Authentication
run_test "Authentication Test" "001" "PASS $PASSWORD\r\nNICK testuser\r\nUSER testuser 0 * :Test User\r\n"

# Test 2: Invalid Password
run_test "Invalid Password Test" "464" "PASS wrongpassword\r\nNICK testuser\r\nUSER testuser 0 * :Test User\r\n"

# Test 3: Channel Join
run_test "Channel Join Test" "JOIN" "PASS $PASSWORD\r\nNICK testuser2\r\nUSER testuser2 0 * :Test User\r\nJOIN #testchannel\r\n"

# Test 4: Empty Password
run_test "Empty Password Test" "464" "PASS \r\nNICK testuser\r\nUSER testuser 0 * :Test User\r\n"

# Test 5: Message to Channel
run_test "Channel Message Test" "PRIVMSG" "PASS $PASSWORD\r\nNICK testuser3\r\nUSER testuser3 0 * :Test User\r\nJOIN #testchannel\r\nPRIVMSG #testchannel :Hello\r\n"

echo -e "\n${YELLOW}Stopping server...${NC}"
cleanup
echo -e "${GREEN}Tests completed${NC}"
