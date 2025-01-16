#!/bin/bash

source $root_dir/config/colors.sh

# Function to show usage
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
