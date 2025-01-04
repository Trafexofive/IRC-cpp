#!/usr/bin/env python3
"""
Enhanced Interactive IRC Client with a TUI (using curses).

Usage:
  python irc_tui.py --server <server> --port <port> --nick <nickname> [OPTIONS]

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
import curses

# Global toggles
DEBUG = False

# Global connection variables
server_socket = None
running = True  # Used to exit main loop
PASSWORD = None
NICK = None
USER = None
REALNAME = None

# Curses windows (created in main)
chat_window = None
input_window = None

def debug_print(message):
    """Print debug output to chat_window if debug mode is enabled."""
    global DEBUG
    if DEBUG:
        safe_addstr(chat_window, f"[DEBUG] {message}\n", curses.color_pair(4))

def safe_addstr(win, text, color_attr=0):
    """
    Safely add text to a curses window, handling possible exceptions.
    Autoscroll if needed.
    """
    try:
        win.addstr(text, color_attr)
        win.refresh()
    except curses.error:
        pass

def slash_help():
    """Print slash commands help to chat_window."""
    help_text = (
        "[Slash Commands]\n"
        "  /auto                  -> Sends PASS (if set), NICK, USER\n"
        "  /password <password>   -> Sets PASS <password>\n"
        "  /nickname <nickname>   -> Sends NICK <nickname>\n"
        "  /login <username>      -> Sends USER <username> 0 * :Real Name\n"
        "  /channel <channel>     -> Sends JOIN <channel>\n"
        "  /ping <target>         -> Sends PING <target>\n"
        "  /msg <target> <msg>    -> Sends a private message to <target>\n"
        "  /exit                  -> Exits the program\n"
        "  /commands              -> Displays this help\n"
    )
    safe_addstr(chat_window, help_text + "\n", curses.color_pair(3))

def send_line(line):
    """
    Send a line to the IRC server socket, appending CR/LF if needed.
    """
    global server_socket
    if not server_socket:
        return
    if not line.endswith('\r\n'):
        line += '\r\n'
    try:
        server_socket.sendall(line.encode('utf-8'))
        debug_print(f"Sent: {line.strip()}")
    except Exception as e:
        safe_addstr(chat_window, f"[Error] Failed to send line: {e}\n", curses.color_pair(2))

def handle_slash_command(cmd_input):
    """
    Handle slash commands. Also updates global variables if needed.
    """
    global PASSWORD, NICK, USER, REALNAME, running
    parts = cmd_input.strip().split(maxsplit=1)
    command = parts[0].lower()  # e.g., /auto
    args = parts[1] if len(parts) > 1 else ""

    if command == "/exit":
        running = False

    elif command == "/commands":
        slash_help()

    elif command == "/auto":
        if PASSWORD:
            send_line(f"PASS {PASSWORD}")
        else:
            safe_addstr(chat_window,
                        "[Warning] No password set. Use --password or /password <pass>\n",
                        curses.color_pair(2))
        send_line(f"NICK {NICK}")
        send_line(f"USER {USER} 0 * :{REALNAME}")

    elif command == "/password":
        if args:
            PASSWORD = args
            send_line(f"PASS {PASSWORD}")
        else:
            safe_addstr(chat_window,
                        "[Error] Usage: /password <password>\n",
                        curses.color_pair(2))

    elif command == "/nickname":
        if args:
            send_line(f"NICK {args}")
        else:
            safe_addstr(chat_window,
                        "[Error] Usage: /nickname <nickname>\n",
                        curses.color_pair(2))

    elif command == "/login":
        if args:
            send_line(f"USER {args} 0 * :{REALNAME}")
        else:
            safe_addstr(chat_window,
                        "[Error] Usage: /login <username>\n",
                        curses.color_pair(2))
    elif command == "/channel":
        if args:
            send_line(f"JOIN {args}")
        else:
            safe_addstr(chat_window,
                        "[Error] Usage: /channel <channel>\n",
                        curses.color_pair(2))
    elif command == "/ping":
        if args:
            send_line(f"PING {args}")
        else:
            safe_addstr(chat_window,
                        "[Error] Usage: /ping <target>\n",
                        curses.color_pair(2))
    elif command == "/msg":
        subparts = args.split(maxsplit=1)
        if len(subparts) == 2:
            target, message = subparts
            send_line(f"PRIVMSG {target} :{message}")
        else:
            safe_addstr(chat_window,
                        "[Error] Usage: /msg <target> <message>\n",
                        curses.color_pair(2))
    else:
        safe_addstr(chat_window,
                    f"[Error] Unknown slash command: {command}\n",
                    curses.color_pair(2))
        slash_help()

def server_reader():
    """
    Thread function to continuously read from the IRC server and display messages.
    """
    global running, server_socket
    while running:
        if not server_socket:
            break
        try:
            data = server_socket.recv(4096)
            if not data:
                safe_addstr(chat_window,
                            "[Error] Server disconnected or connection closed.\n",
                            curses.color_pair(2))
                running = False
                break
            message = data.decode('utf-8', errors='replace').rstrip('\r\n')
            if message:
                safe_addstr(chat_window, f"[Server]: {message}\n")
        except Exception as e:
            safe_addstr(chat_window, f"[Error] Reading from server: {e}\n", curses.color_pair(2))
            running = False
            break

def tui_main(stdscr, args):
    """
    Main function that runs inside curses.wrapper. Sets up windows and event loop.
    """
    global DEBUG, PASSWORD, NICK, USER, REALNAME, server_socket
    global chat_window, input_window, running

    # Curses initialization
    curses.curs_set(1)  # Make cursor visible in input window
    curses.start_color()
    curses.use_default_colors()

    # Establish color pairs (index, fg, bg)
    curses.init_pair(1, curses.COLOR_RED, -1)       # For error / warnings
    curses.init_pair(2, curses.COLOR_YELLOW, -1)    # For mild warnings
    curses.init_pair(3, curses.COLOR_CYAN, -1)      # For help text
    curses.init_pair(4, curses.COLOR_MAGENTA, -1)   # For debug
    # color_pair(0) is the default color

    # Create windows:
    height, width = stdscr.getmaxyx()
    # Reserve one line at the bottom for input
    chat_window = curses.newwin(height-2, width, 0, 0)
    input_window = curses.newwin(1, width, height-1, 0)

    # Initial instructions
    safe_addstr(chat_window, f"[Info] Connecting to {args.server}:{args.port}...\n")

    # Connect to IRC server
    try:
        server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        server_socket.connect((args.server, args.port))
        safe_addstr(chat_window, f"[Info] Connected to {args.server}:{args.port}\n", curses.color_pair(3))
    except Exception as e:
        safe_addstr(chat_window, f"[Error] Connection failed: {e}\n", curses.color_pair(1))
        running = False
        return

    # Send initial PASS, NICK, and USER if password is set
    if PASSWORD:
        send_line(f"PASS {PASSWORD}")
    send_line(f"NICK {NICK}")
    send_line(f"USER {USER} 0 * :{REALNAME}")

    # Start the thread that reads from the server
    thread = threading.Thread(target=server_reader, daemon=True)
    thread.start()

    # Display usage
    safe_addstr(chat_window, "[Info] Type '/commands' for help. Type '/exit' to quit.\n\n", curses.color_pair(3))

    # Main loop to handle user input
    while running:
        input_window.clear()
        input_window.refresh()
        try:
            curses.echo()
            user_input = input_window.getstr(0, 0).decode('utf-8').strip()
            curses.noecho()
        except KeyboardInterrupt:
            running = False
            break
        except EOFError:
            running = False
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
    safe_addstr(chat_window, "[Info] Disconnected from server.\n", curses.color_pair(3))
    chat_window.refresh()
    # Give a moment for the UI to display the disconnect message
    curses.napms(1000)

def main():
    """
    Standard main entry point — parse arguments, then launch curses wrapper.
    """
    parser = argparse.ArgumentParser(
        description="Enhanced Interactive IRC Client with a TUI."
    )
    parser.add_argument("-s", "--server", default="localhost", help="Server hostname/IP")
    parser.add_argument("-p", "--port", type=int, default=22200, help="Server port")
    parser.add_argument("-n", "--nick", default="myNickname", help="Nickname to use")
    parser.add_argument("-u", "--user", default="myUsername", help="USER field")
    parser.add_argument("-r", "--real", default="myRealName", help='Real name')
    parser.add_argument("-P", "--password", default=None, help="IRC password (optional)")
    parser.add_argument("-d", "--debug", action="store_true", help="Enable debug output")

    args = parser.parse_args()

    global DEBUG, PASSWORD, NICK, USER, REALNAME
    DEBUG = args.debug
    PASSWORD = args.password
    NICK = args.nick
    USER = args.user
    REALNAME = args.real

    # Handle ctrl+C to exit gracefully
    def sigint_handler(sig, frame):
        global running
        running = False

    signal.signal(signal.SIGINT, sigint_handler)

    curses.wrapper(tui_main, args)

if __name__ == "__main__":
    main()
