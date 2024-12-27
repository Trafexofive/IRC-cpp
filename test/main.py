# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    main.py                                            :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: mlamkadm <mlamkadm@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/12/27 09:10:57 by mlamkadm          #+#    #+#              #
#    Updated: 2024/12/27 09:10:57 by mlamkadm         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

#!/usr/bin/env python3

import socket
import time
import sys
import subprocess
import os
import signal
from datetime import datetime

def main():
    # Configuration
    HOST = 'localhost'
    PORT = 12600
    PASSWORD = "Alilepro135!"
    SERVER_BINARY = "./bin/irc-server"

    # Parse command line arguments
    if len(sys.argv) > 1:
        PORT = int(sys.argv[1])
    if len(sys.argv) > 2:
        PASSWORD = sys.argv[2]

    # Print header
    print(f"\033[1;34mIRC Server Test Script")
    print(f"Current Date and Time (UTC): {datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S')}")
    print(f"Testing server at {HOST}:{PORT}")
    print(f"Using password: {PASSWORD}\033[0m")
    print("-" * 50)

    # Check if server binary exists
    if not os.path.exists(SERVER_BINARY):
        print(f"\033[0;31mError: Server binary not found at {SERVER_BINARY}\033[0m")
        sys.exit(1)

    # Start server
    print("\n\033[1;33mStarting IRC server...\033[0m")
    try:
        cmd = f"{SERVER_BINARY} {PORT} {PASSWORD}"
        server_process = subprocess.Popen(
            cmd.split(),
            stdout=subprocess.DEVNULL,
            stderr=subprocess.DEVNULL,
            preexec_fn=os.setpgrp
        )
        
        # Wait for server to start
        time.sleep(2)

        # Test connection
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        if sock.connect_ex((HOST, PORT)) == 0:
            print("\033[0;32m✓ Server started successfully\033[0m")
            sock.close()
        else:
            print("\033[0;31m✗ Server failed to start\033[0m")
            server_process.terminate()
            sys.exit(1)

        # Run tests
        print("\n\033[1;33mRunning tests...\033[0m")
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.connect((HOST, PORT))

        # Test authentication
        print("\nTesting authentication...")
        sock.send(f"PASS {PASSWORD}\r\n".encode())
        print(sock.recv(1024).decode())
        
        sock.send("NICK testuser\r\n".encode())
        print(sock.recv(1024).decode())
        
        sock.send("USER testuser 0 * :Test User\r\n".encode())
        print(sock.recv(1024).decode())

        # Test channel operations
        print("\nTesting channel operations...")
        sock.send("JOIN #testchannel\r\n".encode())
        print(sock.recv(1024).decode())

        sock.send("PRIVMSG #testchannel :Hello, World!\r\n".encode())
        print(sock.recv(1024).decode())

        # Clean up
        sock.close()
        print("\n\033[0;32m✓ Tests completed\033[0m")

    except Exception as e:
        print(f"\033[0;31mError: {e}\033[0m")
    finally:
        # Stop server
        try:
            os.killpg(os.getpgid(server_process.pid), signal.SIGTERM)
            print("\033[0;32m✓ Server stopped\033[0m")
        except:
            print("\033[0;31m✗ Failed to stop server\033[0m")

if __name__ == "__main__":
    main()
