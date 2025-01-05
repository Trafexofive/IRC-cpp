#!/bin/bash

# -----------------------------------------------------------------------------
# Enhanced IRC Server Test Suite - Default Configuration
# -----------------------------------------------------------------------------

# Add these with your other default configurations
readonly DEFAULT_SUMMARY_FILE="testSummary.md"
SUMMARY_FILE=$DEFAULT_SUMMARY_FILE
TOTAL_TESTS=0
PASSED_TESTS=0
FAILED_TESTS=0 SKIPPED_TESTS=0
TEST_START_TIME=""
TEST_END_TIME=""
# System Defaults
readonly DEFAULT_PORT=22200
readonly DEFAULT_PASSWORD="Alilepro135!"
readonly DEFAULT_SERVER_BINARY="./bin/irc-server"
readonly DEFAULT_TIMEOUT=5
readonly DEFAULT_LOG_FILE="irc_test.log"
readonly DEFAULT_MAX_PARALLEL_TESTS=5

# Runtime Configuration (can be overridden by flags)
PORT=$DEFAULT_PORT
PASSWORD=$DEFAULT_PASSWORD
SERVER_BINARY=$DEFAULT_SERVER_BINARY
TIMEOUT=$DEFAULT_TIMEOUT
LOG_FILE=$DEFAULT_LOG_FILE
MAX_PARALLEL_TESTS=$DEFAULT_MAX_PARALLEL_TESTS

# State Flags (initialized to defaults)
DEBUG=0
VERBOSE=0
QUIET=0
NO_COLOR=0
SKIP_CLEANUP=0
CUSTOM_TESTS=""

# ANSI Colors (can be disabled with --no-color)
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
GRAY='\033[0;90m'
NC='\033[0m'  # No Color
BOLD='\033[1m'

# Test Status Indicators
OK="${GREEN}[OK]${NC}"
KO="${RED}[KO]${NC}"
INFO="${BLUE}[INFO]${NC}"
WARN="${YELLOW}[WARN]${NC}"

# Function to generate test summary
function generate_summary() {
    local duration=$(($(date +%s) - $(date -d "$TEST_START_TIME" +%s)))
    local hours=$((duration / 3600))
    local minutes=$(( (duration % 3600) / 60 ))
    local seconds=$((duration % 60))
    
    cat > "$SUMMARY_FILE" << EOF
# IRC Server Test Summary
## Test Execution Details

- **Date**: $TEST_START_TIME
- **Duration**: ${hours}h ${minutes}m ${seconds}s
- **User**: $(whoami)
- **Host**: $(hostname)

## Configuration
- Server Port: $PORT
- Debug Mode: $([ "$DEBUG" -eq 1 ] && echo "Enabled" || echo "Disabled")
- Verbose Mode: $([ "$VERBOSE" -eq 1 ] && echo "Enabled" || echo "Disabled")
- Test File: ${CUSTOM_TESTS:-"None"}

## Test Results
- **Total Tests**: $TOTAL_TESTS
- **Passed**: $PASSED_TESTS 🟢
- **Failed**: $FAILED_TESTS 🔴
- **Skipped**: $SKIPPED_TESTS ⚪
- **Success Rate**: $(( (PASSED_TESTS * 100) / (TOTAL_TESTS - SKIPPED_TESTS) ))%

## Failed Tests
\`\`\`
EOF

    # Append failed tests if any
    if [ ${#FAILED_TESTS_ARRAY[@]} -gt 0 ]; then
        printf '%s\n' "${FAILED_TESTS_ARRAY[@]}" >> "$SUMMARY_FILE"
    else
        echo "No failed tests" >> "$SUMMARY_FILE"
    fi

    echo -e "\`\`\`\n" >> "$SUMMARY_FILE"

    # Add system information
    cat >> "$SUMMARY_FILE" << EOF
## System Information
\`\`\`
OS: $(uname -s)
Kernel: $(uname -r)
Architecture: $(uname -m)
Memory: $(free -h | awk '/^Mem:/ {print $2}' 2>/dev/null || echo "N/A")
\`\`\`

## Log Files
- Test Log: $LOG_FILE
- Server Log: server.log
- Summary File: $SUMMARY_FILE

EOF

    # Add error summary if exists
    if [ -f "error_summary.txt" ]; then
        echo -e "\n## Error Summary" >> "$SUMMARY_FILE"
        echo -e "\`\`\`" >> "$SUMMARY_FILE"
        cat "error_summary.txt" >> "$SUMMARY_FILE"
        echo -e "\`\`\`" >> "$SUMMARY_FILE"
    fi
}

# Initialize log file with header information
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

# Function to print a separator line
function print_separator() {
    if [ "${QUIET:-0}" -eq 0 ]; then
        echo -e "${GRAY}----------------------------------------${NC}"
    fi
    echo "----------------------------------------" >> "$LOG_FILE"
}

# Logging function
function log_message() {
    local timestamp
    timestamp=$(date -u '+%Y-%m-%d %H:%M:%S UTC')
    echo "[${timestamp}] $1" >> "$LOG_FILE"
}

# Function to start the IRC server
function start_server() {
    if [ ! -f "$SERVER_BINARY" ]; then
        echo -e "${RED}Error: Server binary not found at $SERVER_BINARY${NC}" >&2
        log_message "ERROR: Server binary not found at $SERVER_BINARY"
        exit 1
    fi

    if [ "${QUIET:-0}" -eq 0 ]; then
        echo -e "\n${YELLOW}Starting IRC server...${NC}"
    fi
    log_message "Starting IRC server"
    "$SERVER_BINARY" "$PORT" "$PASSWORD" > server.log 2>&1 &
    SERVER_PID=$!

    sleep 2  # Wait for server to initialize
    debug_msg "Waiting for server to start (PID: $SERVER_PID)"

    # Verify server is running
    if ! ps -p "$SERVER_PID" > /dev/null; then
        echo -e "${RED}Error: Server failed to start${NC}" >&2
        log_message "ERROR: Server failed to start"
        exit 1
    fi

    if [ "${QUIET:-0}" -eq 0 ]; then
        echo -e "${GREEN}Server started successfully${NC}"
    fi
    log_message "Server started successfully"
}

# Debug message function
function debug_msg() {
    if [ "${DEBUG:-0}" -eq 1 ]; then
        echo -e "${GRAY}[DEBUG] $1${NC}" >&2
        log_message "[DEBUG] $1"
    fi
}

# Function to load and validate custom tests
function load_custom_tests() {
    if [ -n "${CUSTOM_TESTS:-}" ]; then
        if [ ! -f "$CUSTOM_TESTS" ]; then
            echo -e "${RED}Error: Custom test file not found: $CUSTOM_TESTS${NC}" >&2
            log_message "ERROR: Custom test file not found: $CUSTOM_TESTS"
            exit 1
        fi

        debug_msg "Loading custom tests from: $CUSTOM_TESTS"
        log_message "Loading custom tests from: $CUSTOM_TESTS"

        # Verify file is readable
        if [ ! -r "$CUSTOM_TESTS" ]; then
            echo -e "${RED}Error: Cannot read test file: $CUSTOM_TESTS${NC}" >&2
            log_message "ERROR: Cannot read test file: $CUSTOM_TESTS"
            exit 1
        fi
    fi
}


declare -a FAILED_TESTS_ARRAY

function execute_tests() {
    TEST_START_TIME=$(date -u '+%Y-%m-%d %H:%M:%S UTC')
    FAILED_TESTS_ARRAY=()

    if [ ! -f "${CUSTOM_TESTS}" ]; then
        echo -e "${RED}Error: Test file not found: ${CUSTOM_TESTS}${NC}" >&2
        return 1
    fi

    while IFS='|' read -r test_name expected_response commands || [ -n "$test_name" ]; do
        # Skip empty lines and comments
        [[ -z "$test_name" || "$test_name" =~ ^[[:space:]]*# ]] && continue

        ((TOTAL_TESTS++))
        if run_single_test "$test_name" "$expected_response" "$commands"; then
            ((PASSED_TESTS++))
        else
            ((FAILED_TESTS++))
            FAILED_TESTS_ARRAY+=("❌ $test_name - Expected: '$expected_response'")
        fi
    done < "$CUSTOM_TESTS"

    TEST_END_TIME=$(date -u '+%Y-%m-%d %H:%M:%S UTC')
    
    # Generate summary
    generate_summary

    if [ "${QUIET:-0}" -eq 0 ]; then
        echo -e "\n${GREEN}Test Summary:${NC}"
        echo -e "Total: $TOTAL_TESTS"
        echo -e "Passed: ${GREEN}$PASSED_TESTS${NC}"
        echo -e "Failed: ${RED}$FAILED_TESTS${NC}"
        echo -e "Summary written to: ${BLUE}$SUMMARY_FILE${NC}"
    fi
}

function print_short_summary() {
    if [ "${QUIET:-0}" -eq 0 ]; then
        echo -e "\n${BOLD}Test Execution Summary:${NC}"
        echo -e "${GRAY}----------------------------------------${NC}"
        echo -e "Total Tests:    ${BLUE}$TOTAL_TESTS${NC}"
        echo -e "Passed:         ${GREEN}$PASSED_TESTS${NC}"
        echo -e "Failed:         ${RED}$FAILED_TESTS${NC}"
        echo -e "Success Rate:   ${BLUE}$(( (PASSED_TESTS * 100) / (TOTAL_TESTS - SKIPPED_TESTS) ))%${NC}"
        echo -e "Duration:       ${BLUE}$(($(date +%s) - $(date -d "$TEST_START_TIME" +%s))) seconds${NC}"
        echo -e "${GRAY}----------------------------------------${NC}"
        echo -e "Detailed summary: ${BLUE}$SUMMARY_FILE${NC}"
    fi
}

function run_single_test() {
    local test_name=$1
    local expected=$2
    local commands=$3

    if [ "${QUIET:-0}" -eq 0 ]; then
        echo -e "\n${BOLD}Running: ${test_name}${NC}"
    fi
    log_message "Running test: ${test_name}"

    # Use DEFAULT_PASSWORD for AUTH
    if [[ "$commands" == AUTH* ]]; then
        commands="PASS ${DEFAULT_PASSWORD}\nNICK tester\nUSER tester 0 * :Test User"
    fi

    # Execute test and return result
    if send_and_receive "$commands" "$expected"; then
        echo -e "${OK} Test passed: ${test_name}"
        ((PASSED_TESTS++))
        return 0
    else
        echo -e "${KO} Test failed: ${test_name}"
        ((FAILED_TESTS++))
        return 1
    fi
}

# Approximate diff function
function approximate_diff_percentage() {
    # Takes two file paths, calculates line-based differences, and returns an approximate percentage
    local oldFile="$1"
    local newFile="$2"
    local oldLines=$(wc -l < "$oldFile")
    local newLines=$(wc -l < "$newFile")
    local maxLines=$(( oldLines > newLines ? oldLines : newLines ))

    # Count changed lines using diff in unified format,
    # grepping for lines that begin with + or -.
    local diffLines=$(diff -u --ignore-all-space "$oldFile" "$newFile" | grep -E '^[+-]' | wc -l)

    # Calculate percentage of changed lines relative to the larger file
    local percentage=0
    if [ $maxLines -gt 0 ]; then
        percentage=$(( 100 * diffLines / maxLines ))
    fi

    echo "$percentage"
}

# Modified send_and_receive function
function send_and_receive() {
    local commands=$1
    local expected=$2
    local response

    # Create temporary files for the expected string and the server response
    local tmpExpected=$(mktemp)
    local tmpResponse=$(mktemp)

    # Write the expected string to a file for diff calculation
    echo -e "$expected" > "$tmpExpected"

    debug_msg "Sending commands: $commands"
    
    # Send commands to server and capture response in a file
    echo -e "$commands" | nc localhost "$PORT" > "$tmpResponse" 2>/dev/null &
    local nc_pid=$!
    
    # Wait for response with a timeout
    local timeout_counter=0
    while [ $timeout_counter -lt "$TIMEOUT" ] && kill -0 $nc_pid 2>/dev/null; do
        sleep 1
        ((timeout_counter++))
    done

    # Kill nc if it's still running
    kill $nc_pid 2>/dev/null || true

    response=$(<"$tmpResponse")

    if [ "${VERBOSE:-0}" -eq 1 ]; then
        # Print the expected and actual responses
        echo -e "${GRAY}Expected Response:${NC}\n$expected"
        echo -e "${GRAY}Server Response:${NC}\n$response"

        # Calculate and display the approximate difference percentage
        local diffPercent=$(approximate_diff_percentage "$tmpExpected" "$tmpResponse")
        echo -e "${GRAY}Approx. difference between expected and actual:${NC} ${diffPercent}%"
    fi

    # Clean up
    rm -f "$tmpExpected" "$tmpResponse"

    # Check if the expected response is in the actual response
    if [[ "$response" == *"$expected"* ]]; then
        return 0
    else
        return 1
    fi
}



# Cleanup function
function cleanup() {
    if [ "${SKIP_CLEANUP:-0}" -eq 0 ] && [ -n "${SERVER_PID:-}" ]; then
        log_message "Stopping server (PID: $SERVER_PID)"
        kill -TERM "$SERVER_PID" 2>/dev/null || true
        wait "$SERVER_PID" 2>/dev/null || true
    fi
}

# Function to display current configuration
function show_config() {
    if [ "${QUIET:-0}" -eq 0 ]; then
        echo -e "\n${BOLD}Current Configuration:${NC}"
        echo -e "${GRAY}----------------------------------------${NC}"
        echo -e "Server Port:        ${BLUE}$PORT${NC}"
        echo -e "Server Binary:      ${BLUE}$SERVER_BINARY${NC}"
        echo -e "Timeout:           ${BLUE}$TIMEOUT${NC} seconds"
        echo -e "Log File:          ${BLUE}$LOG_FILE${NC}"
        echo -e "Debug Mode:        ${BLUE}$([ "$DEBUG" -eq 1 ] && echo "Enabled" || echo "Disabled")${NC}"
        echo -e "Verbose Mode:      ${BLUE}$([ "$VERBOSE" -eq 1 ] && echo "Enabled" || echo "Disabled")${NC}"
        echo -e "Color Output:      ${BLUE}$([ "$NO_COLOR" -eq 0 ] && echo "Enabled" || echo "Disabled")${NC}"
        echo -e "Custom Tests:      ${BLUE}${CUSTOM_TESTS:-None}${NC}"
        echo -e "Skip Cleanup:      ${BLUE}$([ "$SKIP_CLEANUP" -eq 1 ] && echo "Yes" || echo "No")${NC}"
        echo -e "${GRAY}----------------------------------------${NC}\n"
    fi
}
# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        -h|-\?|--help)
            show_usage
            ;;
        -d|--debug)
            DEBUG=1
            shift
            ;;
        -p|--port)
            if [ -n "$2" ] && [ ${2:0:1} != "-" ]; then
                PORT=$2
                shift 2
            else
                echo -e "${RED}Error: Argument for $1 is missing${NC}" >&2
                exit 1
            fi
            ;;
        -t|--timeout)
            if [ -n "$2" ] && [ ${2:0:1} != "-" ]; then
                TIMEOUT=$2
                shift 2
            else
                echo -e "${RED}Error: Argument for $1 is missing${NC}" >&2
                exit 1
            fi
            ;;
        -l|--log)
            if [ -n "$2" ] && [ ${2:0:1} != "-" ]; then
                LOG_FILE=$2
                shift 2
            else
                echo -e "${RED}Error: Argument for $1 is missing${NC}" >&2
                exit 1
            fi
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
            if [ -n "$2" ] && [ ${2:0:1} != "-" ]; then
                CUSTOM_TESTS=$2
                shift 2
            else
                echo -e "${RED}Error: Argument for $1 is missing${NC}" >&2
                exit 1
            fi
            ;;
        -s|--skip-cleanup)
            SKIP_CLEANUP=1
            shift
            ;;
        -*|--*)
            echo -e "${RED}Error: Unsupported flag $1${NC}" >&2
            exit 1
            ;;
        *)
            echo -e "${RED}Error: Unknown argument $1${NC}" >&2
            exit 1
            ;;
    esac
done

# Validate required parameters
if [ -n "${CUSTOM_TESTS}" ] && [ ! -f "${CUSTOM_TESTS}" ]; then
    echo -e "${RED}Error: Test file '${CUSTOM_TESTS}' not found${NC}" >&2
    exit 1
fi

if ! [[ "$PORT" =~ ^[0-9]+$ ]] || [ "$PORT" -lt 1 ] || [ "$PORT" -gt 65535 ]; then
    echo -e "${RED}Error: Invalid port number${NC}" >&2
    exit 1
fi

if ! [[ "$TIMEOUT" =~ ^[0-9]+$ ]] || [ "$TIMEOUT" -lt 1 ]; then
    echo -e "${RED}Error: Invalid timeout value${NC}" >&2
    exit 1
fi

# Usage function to display help message
function show_usage() {
    cat << EOF
Usage: $0 [OPTIONS]

Enhanced IRC Server Test Suite

Options:
    -h, --help                Show this help message
    -d, --debug              Enable debug mode with detailed logging
    -p, --port PORT          Specify server port (default: $PORT)
    -t, --timeout SEC        Set command timeout (default: $TIMEOUT)
    -l, --log FILE          Specify log file (default: $LOG_FILE)
    -v, --verbose           Enable verbose output
    -q, --quiet             Suppress all non-error output
    -n, --no-color          Disable colored output
    -f, --test-file FILE    Run custom tests from file
    -s, --skip-cleanup      Don't cleanup server process on exit

Examples:
    $0 -d -p 6667           (Using short options)
    $0 --verbose --timeout 10
    $0 -f custom_tests.txt -v
    $0 -t 5 -p 22200 -v

Report bugs to: your@email.com
EOF
    exit 0
}

# Main execution flow
function main() {
    init_log
    show_config

    if [ "${QUIET:-0}" -eq 0 ]; then
        echo -e "${BLUE}${BOLD}Enhanced IRC Server Test Suite${NC}"
        echo -e "${GRAY}Current Date and Time (UTC): $(date -u '+%Y-%m-%d %H:%M:%S')${NC}"
        echo -e "${GRAY}Current User's Login: $(whoami)${NC}"
        echo -e "${GRAY}Testing server at localhost:$PORT${NC}"
    fi

    log_message "Test suite started"
    print_separator

    start_server
    load_custom_tests
    execute_tests

    TEST_START_TIME=$(date -u '+%Y-%m-%d %H:%M:%S UTC')
    execute_tests
    TEST_END_TIME=$(date -u '+%Y-%m-%d %H:%M:%S UTC')

    generate_summary
    print_short_summary

    if [ "${QUIET:-0}" -eq 0 ]; then
        echo -e "\n${YELLOW}Stopping server...${NC}"
    fi
    log_message "Test suite shutting down"
    cleanup

    print_separator
    if [ "${QUIET:-0}" -eq 0 ]; then
        echo -e "${GREEN}Tests completed successfully${NC}"
        echo -e "${GRAY}Log file: $LOG_FILE${NC}"
        echo -e "${GRAY}Server log: server.log${NC}"
    fi
}

main
