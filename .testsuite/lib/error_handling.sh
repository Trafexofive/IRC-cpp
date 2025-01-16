#!/bin/bash

source $root_dir/config/colors.sh

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
