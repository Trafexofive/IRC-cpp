#!/bin/bash

# Load configuration and libraries
export root_dir=.testsuite

source $root_dir/config/config.sh
source $root_dir/config/colors.sh
source $root_dir/lib/error_handling.sh
source $root_dir/lib/logging.sh
source $root_dir/lib/process_management.sh
source $root_dir/lib/test_execution.sh
source $root_dir/lib/utils.sh
source $root_dir/lib/summary.sh
source $root_dir/lib/validation.sh

SERVER_LOG_FILE="log/server.log"

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
        echo -e "${GRAY}Current Date and Time (UTC): $TIME${NC}"
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
        echo -e "${GRAY}Server log: $SERVER_LOG_FILE${NC}"
        echo -e "${GRAY}Summary file: $SUMMARY_FILE${NC}"
    fi
}

# Set up trap for cleanup
trap cleanup EXIT

# Start the test suite
main
