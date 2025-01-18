#!/bin/bash

source $root_dir/config/colors.sh
source $root_dir/config/config.sh

# Define pretty output functions
function print_header() {
    echo -e "${CYAN}========================================${NC}"
    echo -e "${CYAN} $1 ${NC}"
    echo -e "${CYAN}========================================${NC}"
}

function print_success() {
    echo -e "${GREEN}✔ $1${NC}"
}

function print_failure() {
    echo -e "${RED}✘ $1${NC}"
}

function print_debug() {
    echo -e "${GRAY}>>> $1${NC}"
}

function print_divider() {
    echo -e "${CYAN}----------------------------------------${NC}"
}

function send_and_receive() {
    local commands=$1
    local expected=$2
    local response

    local tmpExpected=$(mktemp)
    local tmpResponse=$(mktemp)

    echo -e "$expected" > "$tmpExpected"
    print_debug "Sending commands: $commands"
    
    echo -e "$commands" | nc localhost "$PORT" > "$tmpResponse" 2>/dev/null &
    local nc_pid=$!
    
    local timeout_counter=0
    while [ $timeout_counter -lt "$TIMEOUT" ] && kill -0 $nc_pid 2>/dev/null; do
        sleep 1
        ((timeout_counter++))
    done

    kill $nc_pid 2>/dev/null || true
    
    # Filter out all numeric responses, MOTD, and welcome messages
    response=$(grep -v "^[0-9][0-9][0-9]" < "$tmpResponse" | grep -v "^:\?morpheus-server\.ddns\.net")

    # Normalize newlines and whitespace for comparison
    local normalized_response=$(echo -n "$response" | tr -d '\r' | sed 's/[[:space:]]*$//' | sed 's/^[[:space:]]*//' | awk '{printf "%s\\r\\n", $0}' | sed 's/\\r\\n$//')
    local normalized_expected=$(echo -n "$expected" | tr -d '\r' | sed 's/[[:space:]]*$//' | sed 's/^[[:space:]]*//' | awk '{printf "%s\\r\\n", $0}' | sed 's/\\r\\n$//')

    if [ "${VERBOSE:-0}" -eq 1 ]; then
        print_divider
        echo -e "${GRAY}Expected Response:${NC}\n$expected"
        echo -e "${GRAY}Server Response:${NC}\n$response"
        echo -e "${GRAY}Normalized Expected (hex):${NC}\n$(echo -n "$normalized_expected" | xxd)"
        echo -e "${GRAY}Normalized Response (hex):${NC}\n$(echo -n "$normalized_response" | xxd)"
        local diffPercent=$(approximate_diff_percentage "$tmpExpected" "$tmpResponse")
        echo -e "${GRAY}Approx. difference between expected and actual:${NC} ${diffPercent}%"
        print_divider
    fi

    rm -f "$tmpExpected" "$tmpResponse"

    if [[ "$normalized_response" == "$normalized_expected" ]]; then
        return 0
    else
        # If strings don't match, show detailed difference
        if [ "${VERBOSE:-0}" -eq 1 ]; then
            echo -e "${GRAY}Detailed difference:${NC}"
            diff <(echo "$normalized_expected") <(echo "$normalized_response") || true
            print_divider
        fi
        return 1
    fi
}

function execute_tests() {
    TEST_START_TIME=$(date -u '+%Y-%m-%d %H:%M:%S UTC')
    FAILED_TESTS_ARRAY=()
    if [ ! -f "${CUSTOM_TESTS}" ]; then
        handle_error "Test file not found: ${CUSTOM_TESTS}"
        return 1
    fi
    print_header "Starting Test Execution"
    
    while IFS='|' read -r test_name expected_response commands || [ -n "$test_name" ]; do
        # Skip empty lines and comments
        [[ -z "$test_name" || "$test_name" =~ ^[[:space:]]*# ]] && continue
        ((TOTAL_TESTS++))
        
        local start_time=$(date +%s)
        if run_single_test "$test_name" "$expected_response" "$commands"; then
            ((PASSED_TESTS++))
            if [ "${QUIET:-0}" -eq 0 ]; then
                print_success "Test passed: ${test_name}"
            fi
        else
            ((FAILED_TESTS++))
            FAILED_TESTS_ARRAY+=("❌ $test_name - Expected: '$expected_response'")
            if [ "${QUIET:-0}" -eq 0 ]; then
                print_failure "Test failed: ${test_name}"
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
    
    print_header "Test Execution Summary"
    echo -e "${CYAN}Total Tests:${NC} $TOTAL_TESTS"
    echo -e "${GREEN}Passed:${NC} $PASSED_TESTS"
    echo -e "${RED}Failed:${NC} $FAILED_TESTS"
    echo -e "${CYAN}Skipped:${NC} $SKIPPED_TESTS"
    echo -e "${CYAN}Success Rate:${NC} ${SUCCESS_RATE}%"
    print_divider
}

function run_single_test() {
    local test_name=$1
    local expected=$2
    local commands=$3
    
    if [ "${QUIET:-0}" -eq 0 ]; then
        print_header "Running Test: ${test_name}"
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

function save_error_summary() {
    error_file="log/error-summary.txt"
    
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

    print_debug "Error summary saved to $error_file"
}

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
