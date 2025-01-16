#!/bin/bash

# -----------------------------------------------------------------------------
# Enhanced IRC Server Test Suite - Default Configuration
# Last Updated: 2025-01-16 20:48:07 UTC
# Author: Trafexofive 
# -----------------------------------------------------------------------------

# Default Configurations
readonly DEFAULT_SUMMARY_FILE="testSummary.md"
readonly DEFAULT_PORT=22200
readonly DEFAULT_PASSWORD="Alilepro135!"
readonly DEFAULT_SERVER_BINARY="./bin/irc-server"
readonly DEFAULT_TIMEOUT=5
readonly DEFAULT_LOG_FILE="irc_test.log"
readonly DEFAULT_MAX_PARALLEL_TESTS=5

# Runtime Configuration
SUMMARY_FILE=$DEFAULT_SUMMARY_FILE
PORT=$DEFAULT_PORT
PASSWORD=$DEFAULT_PASSWORD
SERVER_BINARY=$DEFAULT_SERVER_BINARY
TIMEOUT=$DEFAULT_TIMEOUT
LOG_FILE=$DEFAULT_LOG_FILE
MAX_PARALLEL_TESTS=$DEFAULT_MAX_PARALLEL_TESTS

# Test Counters
TOTAL_TESTS=0
PASSED_TESTS=0
FAILED_TESTS=0
SKIPPED_TESTS=0
TEST_START_TIME=""
TEST_END_TIME=""

# State Flags
DEBUG=0
VERBOSE=0
QUIET=0
NO_COLOR=0
SKIP_CLEANUP=0
CUSTOM_TESTS=""

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
INFO="${BLUE}[INFO]${NC}"
WARN="${YELLOW}[WARN]${NC}"

# Array to store failed tests
declare -a FAILED_TESTS_ARRAY

# Enhanced Error Handling Function
function handle_error() {
    local error_message=$1
    local error_code=${2:-1}
    
    echo -e "${RED}Error: ${error_message}${NC}" >&2
    log_message "ERROR: ${error_message}"
    
    if [ "$DEBUG" -eq 1 ]; then
        local frame=0
        while caller $frame; do
            ((frame++))
        done
    fi
    
    exit "$error_code"
}

# Process Management Functions
function is_process_running() {
    local pid=$1
    [ -n "$pid" ] && kill -0 "$pid" 2>/dev/null
}

function check_port_available() {
    local port=$1
    if ! command -v nc >/dev/null 2>&1; then
        handle_error "netcat (nc) is required but not installed"
    fi

    if nc -z localhost "$port" 2>/dev/null; then
        handle_error "Port $port is already in use"
    fi
    return 0
}

# Logging and Output Functions
function log_message() {
    local timestamp
    timestamp=$(date -u '+%Y-%m-%d %H:%M:%S UTC')
    echo "[${timestamp}] $1" >> "$LOG_FILE"
}

function debug_msg() {
    if [ "${DEBUG:-0}" -eq 1 ]; then
        echo -e "${GRAY}[DEBUG] $1${NC}" >&2
        log_message "[DEBUG] $1"
    fi
}

function print_separator() {
    if [ "${QUIET:-0}" -eq 0 ]; then
        echo -e "${GRAY}----------------------------------------${NC}"
    fi
    echo "----------------------------------------" >> "$LOG_FILE"
}

function init_log() {
    echo "========================================" > "$LOG_FILE"
    echo "IRC Server Test Log - $(date -u '+%Y-%m-%d %H:%M:%S UTC')" >> "$LOG_FILE"
    echo "User: Trafexofive" >> "$LOG_FILE"
    echo "Server: localhost:$PORT" >> "$LOG_FILE"
    echo "Debug Mode: $DEBUG" >> "$LOG_FILE"
    echo "Verbose Mode: $VERBOSE" >> "$LOG_FILE"
    echo "========================================" >> "$LOG_FILE"
    echo "" >> "$LOG_FILE"
}

# Test Execution and Validation Functions
function validate_test_file() {
    local test_file=$1
    local line_number=0
    local has_errors=0

    while IFS='|' read -r test_name expected_response commands || [ -n "$test_name" ]; do
        ((line_number++))
        
        [[ -z "$test_name" || "$test_name" =~ ^[[:space:]]*# ]] && continue
        
        if [[ -z "$test_name" || -z "$expected_response" || -z "$commands" ]]; then
            echo -e "${RED}Error in test file at line $line_number: Missing required fields${NC}" >&2
            log_message "ERROR: Invalid test file format at line $line_number"
            has_errors=1
        fi
    done < "$test_file"

    return $has_errors
}

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
- **User**: Trafexofive
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

    if [ ${#FAILED_TESTS_ARRAY[@]} -gt 0 ]; then
        printf '%s\n' "${FAILED_TESTS_ARRAY[@]}" >> "$SUMMARY_FILE"
    else
        echo "No failed tests" >> "$SUMMARY_FILE"
    fi

    echo -e "\`\`\`\n" >> "$SUMMARY_FILE"

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

    if [ -f "error_summary.txt" ]; then
        echo -e "\n## Error Summary" >> "$SUMMARY_FILE"
        echo -e "\`\`\`" >> "$SUMMARY_FILE"
        cat "error_summary.txt" >> "$SUMMARY_FILE"
        echo -e "\`\`\`" >> "$SUMMARY_FILE"
    fi
}

function start_server() {
    if [ ! -f "$SERVER_BINARY" ]; then
        handle_error "Server binary not found at $SERVER_BINARY"
    fi

    check_port_available "$PORT"

    if [ "${QUIET:-0}" -eq 0 ]; then
        echo -e "\n${YELLOW}Starting IRC server...${NC}"
    fi
    log_message "Starting IRC server"
    
    "$SERVER_BINARY" "$PORT" "$PASSWORD" > server.log 2>&1 &
    SERVER_PID=$!

    sleep 2  # Wait for server to initialize
    debug_msg "Waiting for server to start (PID: $SERVER_PID)"

    if ! is_process_running "$SERVER_PID"; then
        handle_error "Server failed to start"
    fi

    if [ "${QUIET:-0}" -eq 0 ]; then
        echo -e "${GREEN}Server started successfully${NC}"
    fi
    log_message "Server started successfully"
}

function send_and_receive() {
    local commands=$1
    local expected=$2
    local response

    local tmpExpected=$(mktemp)
    local tmpResponse=$(mktemp)

    echo -e "$expected" > "$tmpExpected"
    debug_msg "Sending commands: $commands"
    
    echo -e "$commands" | nc localhost "$PORT" > "$tmpResponse" 2>/dev/null &
    local nc_pid=$!
    
    local timeout_counter=0
    while [ $timeout_counter -lt "$TIMEOUT" ] && kill -0 $nc_pid 2>/dev/null; do
        sleep 1
        ((timeout_counter++))
    done

    kill $nc_pid 2>/dev/null || true
    response=$(<"$tmpResponse")

    if [ "${VERBOSE:-0}" -eq 1 ]; then
        echo -e "${GRAY}Expected Response:${NC}\n$expected"
        echo -e "${GRAY}Server Response:${NC}\n$response"
        
        local diffPercent=$(approximate_diff_percentage "$tmpExpected" "$tmpResponse")
        echo -e "${GRAY}Approx. difference between expected and actual:${NC} ${diffPercent}%"
    fi

    rm -f "$tmpExpected" "$tmpResponse"

    [[ "$response" == *"$expected"* ]]
}

# Command Line Argument Parsing and Usage
function show_usage() {
    cat << EOF
Usage: $0 [OPTIONS]

Enhanced IRC Server Test Suite
Last Updated: 2025-01-16 20:49:26 UTC
Current User: Trafexofive

Options:
    -h, --help                Show this help message
    -d, --debug              Enable debug mode with detailed logging
    -p, --port PORT          Specify server port (default: $DEFAULT_PORT)
    -t, --timeout SEC        Set command timeout (default: $DEFAULT_TIMEOUT)
    -l, --log FILE          Specify log file (default: $DEFAULT_LOG_FILE)
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

EOF
    exit 0
}

# Cleanup and Process Management
function cleanup() {
    if [ "${SKIP_CLEANUP:-0}" -eq 0 ] && [ -n "${SERVER_PID:-}" ]; then
        log_message "Stopping server (PID: $SERVER_PID)"
        kill -TERM "$SERVER_PID" 2>/dev/null || true
        wait "$SERVER_PID" 2>/dev/null || true
    fi
}

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
# Function to execute tests from the test file
function execute_tests() {
    TEST_START_TIME=$(date -u '+%Y-%m-%d %H:%M:%S UTC')
    FAILED_TESTS_ARRAY=()

    if [ ! -f "${CUSTOM_TESTS}" ]; then
        handle_error "Test file not found: ${CUSTOM_TESTS}"
        return 1
    fi

    debug_msg "Starting test execution from file: ${CUSTOM_TESTS}"
    
    while IFS='|' read -r test_name expected_response commands || [ -n "$test_name" ]; do
        # Skip empty lines and comments
        [[ -z "$test_name" || "$test_name" =~ ^[[:space:]]*# ]] && continue

        ((TOTAL_TESTS++))
        
        if [ "${QUIET:-0}" -eq 0 ]; then
            echo -e "\n${BOLD}Running Test: ${test_name}${NC}"
        fi
        log_message "Running test: ${test_name}"

        local start_time=$(date +%s)
        
        if run_single_test "$test_name" "$expected_response" "$commands"; then
            ((PASSED_TESTS++))
            if [ "${QUIET:-0}" -eq 0 ]; then
                echo -e "${OK} Test passed: ${test_name}"
            fi
        else
            ((FAILED_TESTS++))
            FAILED_TESTS_ARRAY+=("❌ $test_name - Expected: '$expected_response'")
            if [ "${QUIET:-0}" -eq 0 ]; then
                echo -e "${KO} Test failed: ${test_name}"
            fi
        fi
        
        local duration=$(($(date +%s) - start_time))
        log_message "Test '${test_name}' completed in ${duration}s"
    done < "$CUSTOM_TESTS"

    TEST_END_TIME=$(date -u '+%Y-%m-%d %H:%M:%S UTC')
    
    # Prevent division by zero
    if [ $((TOTAL_TESTS - SKIPPED_TESTS)) -eq 0 ]; then
        SUCCESS_RATE=0
    else
        SUCCESS_RATE=$(( (PASSED_TESTS * 100) / (TOTAL_TESTS - SKIPPED_TESTS) ))
    fi
    
    debug_msg "Test execution completed. Success rate: ${SUCCESS_RATE}%"
}

# Function to run a single test
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
        return 0
    else
        return 1
    fi
}

# Function to save error summary
function save_error_summary() {
    local error_file="error_summary.txt"
    
    {
        echo "Error Summary Report"
        echo "==================="
        echo "Generated: $(date -u '+%Y-%m-%d %H:%M:%S UTC')"
        echo "Test Suite User: Trafexofive"
        echo ""
        
        if [ ${#FAILED_TESTS_ARRAY[@]} -eq 0 ]; then
            echo "No errors detected during test execution."
        else
            echo "Failed Tests:"
            printf '%s\n' "${FAILED_TESTS_ARRAY[@]}"
        fi
        
        echo ""
        echo "Test Statistics:"
        echo "- Total Tests: $TOTAL_TESTS"
        echo "- Passed: $PASSED_TESTS"
        echo "- Failed: $FAILED_TESTS"
        echo "- Skipped: $SKIPPED_TESTS"
        if [ $((TOTAL_TESTS - SKIPPED_TESTS)) -gt 0 ]; then
            echo "- Success Rate: $(( (PASSED_TESTS * 100) / (TOTAL_TESTS - SKIPPED_TESTS) ))%"
        else
            echo "- Success Rate: N/A (no tests executed)"
        fi
    } > "$error_file"

    debug_msg "Error summary saved to $error_file"
}

# Function to calculate approximate diff percentage
function approximate_diff_percentage() {
    local oldFile="$1"
    local newFile="$2"
    local oldLines=$(wc -l < "$oldFile")
    local newLines=$(wc -l < "$newFile")
    local maxLines=$(( oldLines > newLines ? oldLines : newLines ))

    local diffLines=$(diff -u --ignore-all-space "$oldFile" "$newFile" | grep -E '^[+-]' | wc -l)

    local percentage=0
    if [ $maxLines -gt 0 ]; then
        percentage=$(( 100 * diffLines / maxLines ))
    fi

    echo "$percentage"
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
                handle_error "Argument for $1 is missing"
            fi
            ;;
        -t|--timeout)
            if [ -n "$2" ] && [ ${2:0:1} != "-" ]; then
                TIMEOUT=$2
                shift 2
            else
                handle_error "Argument for $1 is missing"
            fi
            ;;
        -l|--log)
            if [ -n "$2" ] && [ ${2:0:1} != "-" ]; then
                LOG_FILE=$2
                shift 2
            else
                handle_error "Argument for $1 is missing"
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
                handle_error "Argument for $1 is missing"
            fi
            ;;
        -s|--skip-cleanup)
            SKIP_CLEANUP=1
            shift
            ;;
        -*|--*)
            handle_error "Unsupported flag $1"
            ;;
        *)
            handle_error "Unknown argument $1"
            ;;
    esac
done

# Main execution flow
function main() {
    init_log
    show_config

    if [ "${QUIET:-0}" -eq 0 ]; then
        echo -e "${BLUE}${BOLD}Enhanced IRC Server Test Suite${NC}"
        echo -e "${GRAY}Current Date and Time (UTC): 2025-01-16 20:49:26${NC}"
        echo -e "${GRAY}Current User's Login: Trafexofive${NC}"
        echo -e "${GRAY}Testing server at localhost:$PORT${NC}"
    fi

    log_message "Test suite started"
    print_separator

    if [ -n "$CUSTOM_TESTS" ]; then
        if ! validate_test_file "$CUSTOM_TESTS"; then
            handle_error "Invalid test file format"
        fi
    fi

    start_server
    
    TEST_START_TIME=$(date -u '+%Y-%m-%d %H:%M:%S UTC')
    execute_tests
    TEST_END_TIME=$(date -u '+%Y-%m-%d %H:%M:%S UTC')

    generate_summary
    save_error_summary

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
        echo -e "${GRAY}Summary file: $SUMMARY_FILE${NC}"
    fi
}

# Set up trap for cleanup
trap cleanup EXIT

# Start the test suite
main
