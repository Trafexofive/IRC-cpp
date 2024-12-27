#!/usr/bin/env bash
#
# Enhanced Interactive IRC Client Script (with slash commands)
# This "10x overhaul" version improves code organization, debugging, and adds new commands.
# It allows you to interact with an IRC server in real time using netcat, with slash-based commands.
#
# Usage:
#   ./irc_enhanced.sh --server <server> --port <port> --nick <nickname> [options]
#
# Commands:
#   /auto                  -> Sends PASS (if supplied), then NICK, then USER (default sign-in steps)
#   /password <password>   -> Sends PASS <password>
#   /nickname <nickname>   -> Sends NICK <nickname>
#   /login <username>      -> Sends USER <username> 0 * :Real Name
#   /channel <channel>     -> Sends JOIN <channel>
#   /ping <target>         -> Sends a PING command to <target> (useful for some IRC servers)
#   /msg <target> <msg>    -> Sends a private message to <target> (user or channel)
#   /exit                  -> Exits the script
#   /commands              -> Displays help for slash commands
#
# Any text not starting with '/' is sent as-is to the server.

###############################################################################
# Default Configuration
###############################################################################
SERVER="localhost"
PORT="22200"
NICK="myNickname"
USER="myUsername"
REALNAME="myRealName"
PASSWORD="Alilepro135!"
DEBUG=0
NO_COLOR=0

###############################################################################
# ANSI Colors
###############################################################################
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
GRAY='\033[0;90m'
NC='\033[0m'

###############################################################################
# Functions
###############################################################################

# Show script usage
function show_usage() {
    cat <<EOF
Usage: $0 [OPTIONS]

Enhanced Interactive IRC Client with Slash Commands

Options:
  -s, --server     Server hostname/IP (default: $SERVER)
  -p, --port       Server port (default: $PORT)
  -n, --nick       Nickname to use (default: $NICK)
  -u, --user       USER field (default: $USER)
  -r, --real       Real name field (default: "$REALNAME")
  -P, --password   IRC password (optional, used by /auto if supplied)
  -d, --debug      Enable debug output (prints sent commands)
  -N, --no-color   Disable colored output
  -h, --help       Show this help message

Examples:
  $0 --server irc.example.com --port 6667 --nick TestNick --password "secret" -d
EOF
    exit 0
}

# Print debug messages if debug mode is enabled
function debug_print() {
    [[ $DEBUG -eq 1 ]] && echo -e "${GRAY}[DEBUG] $*${NC}" >&2
}

# Print slash commands help
function slash_help() {
    echo -e "${YELLOW}Slash Commands:${NC}"
    echo "  /auto                  -> Sends PASS (if set), then NICK, then USER (sign-in steps)"
    echo "  /password <password>   -> Sends PASS <password>"
    echo "  /nickname <nickname>   -> Sends NICK <nickname>"
    echo "  /login <username>      -> Sends USER <username> 0 * :Real Name"
    echo "  /channel <channel>     -> Sends JOIN <channel>"
    echo "  /ping <target>         -> Sends a PING command to <target>"
    echo "  /msg <target> <msg>    -> Sends a private message to <target>"
    echo "  /exit                  -> Exits the script"
    echo "  /commands              -> Displays this help message"
}

###############################################################################
# Parse Command-Line Arguments
###############################################################################
while [[ $# -gt 0 ]]; do
    case "$1" in
        -s|--server)
            SERVER="$2"
            shift 2
            ;;
        -p|--port)
            PORT="$2"
            shift 2
            ;;
        -n|--nick)
            NICK="$2"
            shift 2
            ;;
        -u|--user)
            USER="$2"
            shift 2
            ;;
        -r|--real)
            REALNAME="$2"
            shift 2
            ;;
        -P|--password)
            PASSWORD="$2"
            shift 2
            ;;
        -d|--debug)
            DEBUG=1
            shift
            ;;
        -N|--no-color)
            NO_COLOR=1
            RED=''
            GREEN=''
            YELLOW=''
            BLUE=''
            GRAY=''
            NC=''
            shift
            ;;
        -h|--help)
            show_usage
            ;;
        *)
            echo -e "${RED}Unknown option: $1${NC}"
            show_usage
            ;;
    esac
done

###############################################################################
# Banner / Intro
###############################################################################
echo -e "${BLUE}==========================================${NC}"
echo -e "${BLUE}   Enhanced IRC Client with Slash Cmds   ${NC}"
echo -e "${BLUE}==========================================${NC}"
echo -e "${GREEN}Server:   $SERVER:$PORT${NC}"
echo -e "${GREEN}Nickname: $NICK${NC}"
echo -e "${GREEN}User:     $USER${NC}"
echo -e "${BLUE}==========================================${NC}"
echo

###############################################################################
# FIFO Setup & Netcat Launch
###############################################################################
TMP_FIFO=$(mktemp -u /tmp/irc_fifo.XXXXXX)
mkfifo "$TMP_FIFO"

# Start netcat in background; send everything from FIFO to the server, read server messages
nc "$SERVER" "$PORT" < "$TMP_FIFO" | while IFS= read -r line; do
    echo -e "${GREEN}[Server]:${NC} $line"
done &
NC_PID=$!

# Cleanup on exit
function cleanup() {
    kill "$NC_PID" 2>/dev/null || true
    rm -f "$TMP_FIFO"
    echo -e "${YELLOW}Disconnected from server.${NC}"
}
trap cleanup EXIT INT TERM

###############################################################################
# Initial Connection Steps
###############################################################################
if [[ -n "$PASSWORD" ]]; then
    echo "PASS $PASSWORD" > "$TMP_FIFO"
    debug_print "PASS $PASSWORD"
fi
echo "NICK $NICK" > "$TMP_FIFO"
debug_print "NICK $NICK"
echo "USER $USER 0 * :$REALNAME" > "$TMP_FIFO"
debug_print "USER $USER 0 * :$REALNAME"

echo -e "${YELLOW}Type '/commands' for help. Type '/exit' to quit.${NC}"

###############################################################################
# Main Loop: Handle User Input
###############################################################################
while true; do
    echo -n "[You]: "
    read -r user_input
    # If empty input, ignore
    [[ -z "$user_input" ]] && continue

    # Slash command detection
    if [[ "${user_input:0:1}" == "/" ]]; then
        cmd=$(echo "$user_input" | awk '{print $1}' | tr '[:upper:]' '[:lower:]')
        args=$(echo "$user_input" | cut -d ' ' -f2-)

        case "$cmd" in
            "/exit")
                break
                ;;
            "/commands")
                slash_help
                ;;
            "/auto")
                if [[ -n "$PASSWORD" ]]; then
                    echo "PASS $PASSWORD" > "$TMP_FIFO"
                    debug_print "PASS $PASSWORD"
                else
                    echo -e "${RED}No password set. Provide one via --password or /password.<${NC}"
                fi
                echo "NICK $NICK" > "$TMP_FIFO"
                debug_print "NICK $NICK"
                echo "USER $USER 0 * :$REALNAME" > "$TMP_FIFO"
                debug_print "USER $USER 0 * :$REALNAME"
                ;;
            "/password")
                if [[ -n "$args" ]]; then
                    PASSWORD="$args"
                    echo "PASS $PASSWORD" > "$TMP_FIFO"
                    debug_print "PASS $PASSWORD"
                else
                    echo -e "${RED}Usage: /password <password>${NC}"
                fi
                ;;
            "/nickname")
                if [[ -n "$args" ]]; then
                    echo "NICK $args" > "$TMP_FIFO"
                    debug_print "NICK $args"
                else
                    echo -e "${RED}Usage: /nickname <nickname>${NC}"
                fi
                ;;
            "/login")
                if [[ -n "$args" ]]; then
                    echo "USER $args 0 * :$REALNAME" > "$TMP_FIFO"
                    debug_print "USER $args 0 * :$REALNAME"
                else
                    echo -e "${RED}Usage: /login <username>${NC}"
                fi
                ;;
            "/channel")
                if [[ -n "$args" ]]; then
                    echo "JOIN $args" > "$TMP_FIFO"
                    debug_print "JOIN $args"
                else
                    echo -e "${RED}Usage: /channel <channel>${NC}"
                fi
                ;;
            "/ping")
                # PING <target> - Some servers expect server or token here
                # We'll just send a raw PING command with your argument
                if [[ -n "$args" ]]; then
                    echo "PING $args" > "$TMP_FIFO"
                    debug_print "PING $args"
                else
                    echo -e "${RED}Usage: /ping <target>${NC}"
                fi
                ;;
            "/msg")
                # Send a private message to a user or channel
                # Usage:  /msg <target> <message>
                target=$(echo "$args" | awk '{print $1}')
                message=$(echo "$args" | cut -d ' ' -f2-)
                if [[ -n "$target" && -n "$message" ]]; then
                    echo "PRIVMSG $target :$message" > "$TMP_FIFO"
                    debug_print "PRIVMSG $target :$message"
                else
                    echo -e "${RED}Usage: /msg <target> <message>${NC}"
                fi
                ;;
            *)
                echo -e "${RED}Unknown slash command: $cmd${NC}"
                slash_help
                ;;
        esac
    else
        # Send the line as-is
        echo "$user_input" > "$TMP_FIFO"
        debug_print "Sent: $user_input"
    fi
done

cleanup
exit 0
