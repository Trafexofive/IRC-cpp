#!/bin/bash

# Function to display help
usage() {
    echo "Usage: $0 -s <server> -p <port> -f <file>"
    echo "  -s, --server    IRC server address"
    echo "  -p, --port      IRC server port"
    echo "  -f, --file      File containing IRC commands"
    echo "  -h, --help      Display this help message"
    exit 1
}

# Color definitions
GREEN='\033[0;32m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Function to test IRC commands from a file
test_irc_file() {
    local server="$1"
    local port="$2"
    local file="$3"
    
    if [[ ! -f "$file" ]]; then
        echo -e "${RED}Error: File '$file' not found.${NC}"
        exit 1
    fi
    
    echo -e "\n${GREEN}Testing IRC commands from file:${NC} $file\n"
    
    # Create a named pipe for netcat input and output
    local input_pipe=$(mktemp -u)
    local output_pipe=$(mktemp -u)
    mkfifo "$input_pipe" "$output_pipe"
    
    # Start netcat in the background with both pipes
    nc "$server" "$port" < "$input_pipe" > "$output_pipe" &
    local nc_pid=$!
    
    # Start background process to handle server responses
    cat "$output_pipe" | while IFS= read -r response; do
        if [[ $response == :* ]]; then
            echo -e "${BLUE}SERVER  >${NC} $response"
        fi
    done &
    local cat_pid=$!
    
    # Open the input pipe for writing
    exec 3>"$input_pipe"
    
    # Process and send commands
    while IFS= read -r line; do
        # Send the exact line to the server, appending \r\n
        echo -e "${GREEN}SENDING >${NC} '$line'"
        echo -e "$line\r\n" >&3
        sleep 1  # Give the server time to respond
    done < "$file"
    
    # Clean up
    exec 3>&-
    kill $nc_pid $cat_pid 2>/dev/null
    wait $nc_pid $cat_pid 2>/dev/null
    rm "$input_pipe" "$output_pipe"
    echo # Add newline at end
}

# Parse command-line arguments
while [[ "$#" -gt 0 ]]; do
    case $1 in
        -s|--server) SERVER="$2"; shift ;;
        -p|--port) PORT="$2"; shift ;;
        -f|--file) FILE="$2"; shift ;;
        -h|--help) usage ;;
        *) echo "Unknown parameter: $1"; usage ;;
    esac
    shift
done

# Validate required arguments
if [[ -z "$SERVER" || -z "$PORT" || -z "$FILE" ]]; then
    echo -e "${RED}Error: Server, port, and file are required.${NC}"
    usage
fi

# Test IRC commands from the file
test_irc_file "$SERVER" "$PORT" "$FILE"
