#!/bin/bash

source $root_dir/config/colors.sh
source $root_dir/config/config.sh

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
