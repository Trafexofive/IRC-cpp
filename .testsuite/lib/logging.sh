#!/bin/bash

source $root_dir/config/colors.sh

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
