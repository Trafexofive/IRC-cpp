#!/bin/bash

# -----------------------------------------------------------------------------
# IRC Server Test Suite - Based on a Verified Working Version
# -----------------------------------------------------------------------------
# This script runs a series of tests against a running IRC server. It verifies
# connections, password authentication, joining channels, and sending messages.
# -----------------------------------------------------------------------------

# Default Configuration
PORT=22200
PASSWORD="Alilepro135!"
SERVER_BINARY="./bin/irc-server"
TIMEOUT=5
LOG_FILE="irc_test.log"
DEBUG=0
VERBOSE=0
QUIET=0
NO_COLOR=0
CUSTOM_TESTS=""
SKIP_CLEANUP=0

# ANSI Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
GRAY='\033[0;90m'
NC='\033[0m'
BOLD='\033[1m'

# Test Status Indicators
OK="${GREEN}[OK]${NC}"
KO="${RED}[KO]${NC}"

# Usage function
function show_usage() {
    cat << EOF
Usage: $0 [OPTIONS]

IRC Server Test Suite

Options:
    -h, --help                Show this help message
    -d, --debug               Enable debug mode with detailed logging
    -p, --port PORT           Specify server port (default: $PORT)
    -t, --timeout SEC         Set command timeout (default: $TIMEOUT)
    -l, --log FILE            Specify log file (default: $LOG_FILE)
    -v, --verbose             Enable verbose output
    -q, --quiet               Suppress all non-error output
    -n, --no-color            Disable colored output
    -f, --test-file FILE      Run custom tests from file
    -s, --skip-cleanup        Don't cleanup server process on exit

Examples:
    $0 --debug --port 6667
    $0 --verbose --timeout 10
    $0 --test-file custom_tests.txt

Report bugs to: your@email.com
EOF
    exit 0
}

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        -h|--help)
            show_usage
            ;;
        -d|--debug)
            DEBUG=1
            shift
            ;;
        -p|--port)
            PORT="$2"
            shift 2
            ;;
        -t|--timeout)
            TIMEOUT="$2"
            shift 2
            ;;
        -l|--log)
            LOG_FILE="$2"
            shift 2
            ;;
        -v|--verbose)
            VERBOSE=1
            shift
            ;;
        -q|--quiet)
            QUIET=1
            shift
            ;;
        -n|--no-color)
            NO_COLOR=1
            RED=''
            GREEN=''
            YELLOW=''
            BLUE=''
            GRAY=''
            NC=''
            BOLD=''
            shift
            ;;
        -f|--test-file)
            CUSTOM_TESTS="$2"
            shift 2
            ;;
        -s|--skip-cleanup)
            SKIP_CLEANUP=1
            shift
            ;;
        *)
            echo "Unknown option: $1"
            show_usage
            ;;
    esac
done

# Debug function
function debug_msg() {
    if [ $DEBUG -eq 1 ]; then
        echo -e "${GRAY}[DEBUG] $1${NC}" >&2
        log_message "[DEBUG] $1"
    fi
}

# Verbose function
function verbose_msg() {
    if [ $VERBOSE -eq 1 ] && [ $QUIET -eq 0 ]; then
        echo -e "${GRAY}$1${NC}"
        log_message "[VERBOSE] $1"
    fi
}

# Initialize log file
function init_log() {
    echo "========================================" > "$LOG_FILE"
    echo "IRC Server Test Log - $(date -u '+%Y-%m-%d %H:%M:%S UTC')" >> "$LOG_FILE"
    echo "User: $(whoami)" >> "$LOG_FILE"
    echo "Server: localhost:$PORT" >> "$LOG_FILE"
    echo "Debug Mode: $DEBUG" >> "$LOG_FILE"
    echo "Verbose Mode: $VERBOSE" >> "$LOG_FILE"
    echo "========================================" >> "$LOG_FILE"
    echo "" >> "$LOG_FILE"
}

function log_message() {
    local timestamp
    timestamp=$(date -u '+%Y-%m-%d %H:%M:%S UTC')
    echo "[${timestamp}] $1" >> "$LOG_FILE"
}

function print_separator() {
    [ $QUIET -eq 0 ] && echo -e "${GRAY}----------------------------------------${NC}"
    echo "----------------------------------------" >> "$LOG_FILE"
}

# send_and_receive Function
# Sends the provided commands to the server and checks if the output
# contains the expected value. It uses a temporary file to store output.
function send_and_receive() {
    local commands=$1
    local expected=$2
    local response=""
    
    local tmpfile
    tmpfile=$(mktemp)
    
    debug_msg "Created temporary file: $tmpfile"
    log_message "Sending commands:"
    log_message "$commands"
    
    # Pipe commands into netcat with a small sleep to allow server processing
    (echo -e "$commands"; sleep 1) \
        | timeout $TIMEOUT nc localhost $PORT > "$tmpfile" 2>> "$LOG_FILE"
    local exit_code=$?
    response=$(<"$tmpfile")
    
    debug_msg "Raw response size: $(wc -c < "$tmpfile") bytes"
    log_message "Raw response (hex dump):"
    log_message "$(cat "$tmpfile" | xxd)"
    log_message "Exit code: $exit_code"
    
    rm -f "$tmpfile"
    debug_msg "Removed temporary file: $tmpfile"
    
    if [[ "$response" == *"$expected"* ]]; then
        log_message "Response matched expected pattern: '$expected'"
        return 0
    else
        log_message "Response did not match expected pattern: '$expected'"
        return 1
    fi
}

# run_test Function
# Executes a test by sending commands and checking for expected output.
function run_test() {
    local test_name=$1
    local expected=$2
    local commands=$3
    
    [ $QUIET -eq 0 ] && echo -e "\n${BOLD}Running: ${test_name}${NC}"
    log_message "Starting test: ${test_name}"
    print_separator
    
    verbose_msg "Sending:\n$commands"
    debug_msg "Expected pattern: '$expected'"
    
    if send_and_receive "$commands" "$expected"; then
        [ $QUIET -eq 0 ] && echo -e "${OK} Test passed"
        log_message "Test PASSED"
        return 0
    else
        [ $QUIET -eq 0 ] && {
            echo -e "${KO} Test failed"
            echo -e "${YELLOW}Expected:${NC} $expected"
            echo -e "${RED}Actual response did not contain expected string${NC}"
        }
        log_message "Test FAILED"
        log_message "Expected: $expected"
        return 1
    fi
}

# load_custom_tests Function
# If a custom test file is provided, source it to load additional tests.
function load_custom_tests() {
    if [ -n "$CUSTOM_TESTS" ] && [ -f "$CUSTOM_TESTS" ]; then
        debug_msg "Loading custom tests from: $CUSTOM_TESTS"
        source "$CUSTOM_TESTS"
    fi
}

# cleanup Function
# Stops the server, if running, unless skip-cleanup is enabled.
function cleanup() {
    if [ $SKIP_CLEANUP -eq 0 ] && [ -n "${SERVER_PID:-}" ]; then
        log_message "Stopping server (PID: $SERVER_PID)"
        kill -9 "$SERVER_PID" 2>/dev/null
        wait "$SERVER_PID" 2>/dev/null || true
    fi
    log_message "Test suite completed"
    [ $QUIET -eq 0 ] && echo -e "${GRAY}Full test log available in: $LOG_FILE${NC}"
}

trap cleanup EXIT INT TERM

# Initialize log file
init_log

# Print header
if [ $QUIET -eq 0 ]; then
    echo -e "${BLUE}${BOLD}IRC Server Test Suite${NC}"
    echo "Current Date and Time (UTC): $(date -u '+%Y-%m-%d %H:%M:%S')"
    echo "Current User's Login: $(whoami)"
    echo "Testing server at localhost:$PORT"
fi

log_message "Test suite started"
print_separator

# Check if server binary exists
if [ ! -f "$SERVER_BINARY" ]; then
    echo -e "${RED}Error: Server binary not found at $SERVER_BINARY${NC}" >&2
    log_message "ERROR: Server binary not found at $SERVER_BINARY"
    exit 1
fi

# Start server in background
[ $QUIET -eq 0 ] && echo -e "\n${YELLOW}Starting IRC server...${NC}"
log_message "Starting IRC server"
"$SERVER_BINARY" "$PORT" "$PASSWORD" > server.log 2>&1 &
SERVER_PID=$!

# Wait briefly for server to start
sleep 2
debug_msg "Waiting for server to start (PID: $SERVER_PID)"

# Verify server is running 
if ! ps -p "$SERVER_PID" > /dev/null || ! nc -z localhost "$PORT" 2>/dev/null; then
    echo -e "${RED}Error: Server failed to start or is not listening on port $PORT${NC}" >&2
    log_message "ERROR: Server failed to start or is not listening on port $PORT"
    cleanup
    exit 1
fi

[ $QUIET -eq 0 ] && echo -e "${GREEN}Server started successfully${NC}"
log_message "Server started successfully"
print_separator

# Load any custom tests if specified
load_custom_tests

# Run standard tests
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

[ $QUIET -eq 0 ] && echo -e "\n${YELLOW}Stopping server...${NC}"
log_message "Test suite shutting down"
cleanup

print_separator
if [ $QUIET -eq 0 ]; then
    echo -e "${GREEN}Tests completed${NC}"
    echo -e "${GRAY}Log file: $LOG_FILE${NC}"
    echo -e "${GRAY}Server log: server.log${NC}"
fi

exit 0
