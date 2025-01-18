#!/usr/bin/env python3
"""
Enhanced Interactive IRC Client Script

Usage:
  python irc_enhanced.py --server <server> --port <port> --nick <nickname> [OPTIONS]

Slash Commands:
  /auto                  -> Sends PASS (if supplied), then NICK, then USER (default sign-in steps)
  /password <password>   -> Sets the global PASSWORD variable and sends PASS <password>
  /nickname <nickname>   -> Sends NICK <nickname>
  /login <username>      -> Sends USER <username> 0 * :Real Name
  /channel <channel>     -> Sends JOIN <channel>
  /ping <target>         -> Sends PING <target>
  /msg <target> <msg>    -> Sends a private message to <target> (user or channel)
  /exit                  -> Exits the script
  /commands              -> Displays slash commands help

Any line not starting with '/' is sent as-is to the server.
"""

import sys
import socket
import argparse
import threading
import signal

# ANSI color constants (can be disabled via --no-color)
RED = '\033[0;31m'
GREEN = '\033[0;32m'
YELLOW = '\033[1;33m'
BLUE = '\033[0;34m'
GRAY = '\033[0;90m'
NC = '\033[0m'  # No Color

# Global toggles
DEBUG = False
NO_COLOR = False

# Global connection variables
server_socket = None
running = True  # Used to exit the main loop
PASSWORD = None
NICK = None
USER = None
REALNAME = None


def debug_print(*args):
    """Print debug output if debug mode is enabled."""
    if DEBUG:
        print(f"{GRAY}[DEBUG]{NC}", *args)


def slash_help():
    """Print slash commands help."""
    print(f"{YELLOW}Slash Commands:{NC}")
    print("  /auto                  -> Sends PASS (if set), then NICK, then USER (sign-in steps)")
    print("  /password <password>   -> Sends PASS <password>")
    print("  /nickname <nickname>   -> Sends NICK <nickname>")
    print("  /login <username>      -> Sends USER <username> 0 * :Real Name")
    print("  /channel <channel>     -> Sends JOIN <channel>")
    print("  /ping <target>         -> Sends a PING command to <target>")
    print("  /msg <target> <msg>    -> Sends a private message to <target>")
    print("  /exit                  -> Exits this program")
    print("  /commands              -> Displays this help")


def color_print(prefix, message, color=GREEN):
    """Helper to print server/client messages with color."""
    if NO_COLOR:
        print(f"[{prefix}] {message}")
    else:
        print(f"{color}[{prefix}]:{NC} {message}")


def send_line(line):
    """Send a line to the IRC server socket, appending CR/LF if needed."""
    global server_socket
    if not server_socket:
        return
    if not line.endswith('\r\n'):
        line += '\r\n'
    try:
        server_socket.sendall(line.encode('utf-8'))
        debug_print(f"Sent: {line.strip()}")
    except Exception as e:
        color_print("Error", f"Failed to send line: {e}", RED)


def handle_slash_command(cmd_input):
    """Handle slash command input."""
    global PASSWORD, NICK, USER, REALNAME

    parts = cmd_input.strip().split(maxsplit=1)
    command = parts[0].lower()  # e.g., /auto
    args = parts[1] if len(parts) > 1 else ""

    if command == "/exit":
        # Signal main loop to exit
        global running
        running = False

    elif command == "/commands":
        slash_help()

    elif command == "/auto":
        if PASSWORD:
            send_line(f"PASS {PASSWORD}")
        else:
            color_print("Warning", "No password set. Provide one with --password or /password <pass>", RED)
        send_line(f"NICK {NICK}")
        send_line(f"USER {USER} 0 * :{REALNAME}")

    elif command == "/password":
        if args:
            PASSWORD = args
            send_line(f"PASS {PASSWORD}")
        else:
            color_print("Error", "Usage: /password <password>", RED)

    elif command == "/nickname":
        if args:
            send_line(f"NICK {args}")
        else:
            color_print("Error", "Usage: /nickname <nickname>", RED)

    elif command == "/login":
        if args:
            send_line(f"USER {args} 0 * :{REALNAME}")
        else:
            color_print("Error", "Usage: /login <username>", RED)

    elif command == "/channel":
        if args:
            send_line(f"JOIN {args}")
        else:
            color_print("Error", "Usage: /channel <channel>", RED)

    elif command == "/ping":
        if args:
            send_line(f"PING {args}")
        else:
            color_print("Error", "Usage: /ping <target>", RED)

    elif command == "/msg":
        # Usage: /msg <target> <message>
        subparts = args.split(maxsplit=1)
        if len(subparts) == 2:
            target, message = subparts
            send_line(f"PRIVMSG {target} :{message}")
        else:
            color_print("Error", "Usage: /msg <target> <message>", RED)
    else:
        color_print("Error", f"Unknown slash command: {command}", RED)
        slash_help()


def server_reader():
    """Thread function to continuously read from the IRC server."""
    global running, server_socket

    while running:
        if not server_socket:
            break
        try:
            data = server_socket.recv(4096)
            if not data:
                color_print("Error", "Server disconnected or connection closed.", RED)
                running = False
                break
            message = data.decode('utf-8', errors='replace').rstrip('\r\n')
            if message:
                color_print("Server", message)
        except Exception as e:
            color_print("Error", f"Reading from server: {e}", RED)
            running = False
            break


def main():
    """Main program entry point."""
    parser = argparse.ArgumentParser(
        description="Enhanced Interactive IRC Client (Python '10x' version)."
    )
    parser.add_argument("-s", "--server", default="localhost", help="Server hostname/IP")
    parser.add_argument("-p", "--port", type=int, default=6667, help="Server port")
    parser.add_argument("-n", "--nick", default="myNickname", help="Nickname to use")
    parser.add_argument("-u", "--user", default="myUsername", help="USER field")
    parser.add_argument("-r", "--real", default="myRealName", help='Real name')
    parser.add_argument("-P", "--password", default=None, help="IRC password (optional)")
    parser.add_argument("-d", "--debug", action="store_true", help="Enable debug output")
    parser.add_argument("-N", "--no-color", action="store_true", help="Disable colored output")

    args = parser.parse_args()

    global DEBUG, NO_COLOR, PASSWORD, NICK, USER, REALNAME, server_socket
    DEBUG = args.debug
    NO_COLOR = args.no_color
    PASSWORD = args.password
    NICK = args.nick
    USER = args.user
    REALNAME = args.real

    # Connect to IRC server
    try:
        server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        server_socket.connect((args.server, args.port))
        color_print("Info", f"Connected to {args.server}:{args.port}", BLUE)
    except Exception as e:
        color_print("Error", f"Connection to {args.server}:{args.port} failed: {e}", RED)
        sys.exit(1)

    # Send initial PASS, NICK, and USER if password is set
    if PASSWORD:
        send_line(f"PASS {PASSWORD}")
    send_line(f"NICK {NICK}")
    send_line(f"USER {USER} 0 * :{REALNAME}")

    # Start the thread that reads from the server
    thread = threading.Thread(target=server_reader, daemon=True)
    thread.start()

    color_print("Info", "Type '/commands' for command help. Type '/exit' to quit.\n", YELLOW)

    # Ctrl+C handler to exit gracefully
    def sigint_handler(sig, frame):
        color_print("Info", "Exiting...", YELLOW)
        global running
        running = False

    signal.signal(signal.SIGINT, sigint_handler)

    # Main loop to handle user input
    global running
    while running:
        try:
            user_input = input(f"{BLUE}[You]: {NC}").strip()
        except EOFError:
            # If user pressed Ctrl+D or input is closed
            break

        # If empty input, skip
        if not user_input:
            continue

        # Slash commands
        if user_input.startswith("/"):
            handle_slash_command(user_input)
        else:
            # Send raw text to server
            send_line(user_input)

    # Cleanup
    running = False
    if server_socket:
        try:
            server_socket.close()
        except Exception:
            pass
    color_print("Info", "Disconnected from server.", YELLOW)


if __name__ == "__main__":
    main()

