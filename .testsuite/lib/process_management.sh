#!/bin/bash

source $root_dir/config/colors.sh

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

function cleanup() {
    if [ "${SKIP_CLEANUP:-0}" -eq 0 ] && [ -n "${SERVER_PID:-}" ]; then
        log_message "Stopping server (PID: $SERVER_PID)"
        kill -TERM "$SERVER_PID" 2>/dev/null || true
        wait "$SERVER_PID" 2>/dev/null || true
    fi
}
