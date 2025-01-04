# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    main.py                                            :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: mlamkadm <mlamkadm@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/01/03 20:23:52 by mlamkadm          #+#    #+#              #
#    Updated: 2025/01/03 20:23:52 by mlamkadm         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

#!/usr/bin/env python3

import argparse
import os
import yaml
import subprocess
import sys
import tempfile
import time
from datetime import datetime, timezone

# -----------------------------------------------------------------------------
# Enhanced IRC Server Test Suite - Default Configuration
# -----------------------------------------------------------------------------

DEFAULT_SUMMARY_FILE = "testSummary.md"
SUMMARY_FILE = DEFAULT_SUMMARY_FILE
TOTAL_TESTS = 0
PASSED_TESTS = 0
FAILED_TESTS = 0
SKIPPED_TESTS = 0
TEST_START_TIME = None
TEST_END_TIME = None

DEFAULT_PORT = 22200
DEFAULT_PASSWORD = "Alilepro135!"
DEFAULT_SERVER_BINARY = "./bin/irc-server"
DEFAULT_TIMEOUT = 5
DEFAULT_LOG_FILE = "irc_test.log"
DEFAULT_MAX_PARALLEL_TESTS = 5

PORT = DEFAULT_PORT
PASSWORD = DEFAULT_PASSWORD
SERVER_BINARY = DEFAULT_SERVER_BINARY
TIMEOUT = DEFAULT_TIMEOUT
LOG_FILE = DEFAULT_LOG_FILE
MAX_PARALLEL_TESTS = DEFAULT_MAX_PARALLEL_TESTS

DEBUG = False
VERBOSE = False
QUIET = False
NO_COLOR = False
SKIP_CLEANUP = False
CUSTOM_TESTS = ""

RED = '\033[0;31m'
GREEN = '\033[0;32m'
YELLOW = '\033[1;33m'
BLUE = '\033[0;34m'
GRAY = '\033[0;90m'
NC = '\033[0m'
BOLD = '\033[1m'

OK = f"{GREEN}[OK]{NC}"
KO = f"{RED}[KO]{NC}"
INFO = f"{BLUE}[INFO]{NC}"
WARN = f"{YELLOW}[WARN]{NC}"

FAILED_TESTS_ARRAY = []

def generate_summary():
    global TEST_END_TIME
    if TEST_END_TIME is None:
        TEST_END_TIME = datetime.now(timezone.utc)
    duration = int(TEST_END_TIME.timestamp() - TEST_START_TIME.timestamp())
    hours, remainder = divmod(duration, 3600)
    minutes, seconds = divmod(remainder, 60)
    
    with open(SUMMARY_FILE, 'w') as summary_file:
        summary_file.write(f"# IRC Server Test Summary\n")
        summary_file.write(f"## Test Execution Details\n")
        summary_file.write(f"\n")
        summary_file.write(f"- **Date**: {TEST_START_TIME}\n")
        summary_file.write(f"- **Duration**: {hours}h {minutes}m {seconds}s\n")
        summary_file.write(f"- **User**: {os.getlogin()}\n")
        summary_file.write(f"- **Host**: {os.uname().nodename}\n")
        summary_file.write(f"\n")
        summary_file.write(f"## Configuration\n")
        summary_file.write(f"- Server Port: {PORT}\n")
        summary_file.write(f"- Debug Mode: {'Enabled' if DEBUG else 'Disabled'}\n")
        summary_file.write(f"- Verbose Mode: {'Enabled' if VERBOSE else 'Disabled'}\n")
        summary_file.write(f"- Test File: {CUSTOM_TESTS if CUSTOM_TESTS else 'None'}\n")
        summary_file.write(f"\n")
        summary_file.write(f"## Test Results\n")
        summary_file.write(f"- **Total Tests**: {TOTAL_TESTS}\n")
        summary_file.write(f"- **Passed**: {PASSED_TESTS} 🟢\n")
        summary_file.write(f"- **Failed**: {FAILED_TESTS} 🔴\n")
        summary_file.write(f"- **Skipped**: {SKIPPED_TESTS} ⚪\n")
        if TOTAL_TESTS - SKIPPED_TESTS > 0:
            summary_file.write(f"- **Success Rate**: {PASSED_TESTS * 100 / (TOTAL_TESTS - SKIPPED_TESTS):.2f}%\n")
        summary_file.write(f"\n")
        summary_file.write(f"## Failed Tests\n")
        summary_file.write(f"```\n")
        if FAILED_TESTS_ARRAY:
            summary_file.write('\n'.join(FAILED_TESTS_ARRAY))
        else:
            summary_file.write("No failed tests")
        summary_file.write(f"```\n")
        summary_file.write(f"\n")
        summary_file.write(f"## System Information\n")
        summary_file.write(f"```\n")
        summary_file.write(f"OS: {os.uname().sysname}\n")
        summary_file.write(f"Kernel: {os.uname().release}\n")
        summary_file.write(f"Architecture: {os.uname().machine}\n")
        try:
            mem_total = subprocess.check_output("free -h | awk '/^Mem:/ {print $2}'", shell=True).decode().strip()
        except subprocess.CalledProcessError:
            mem_total = "N/A"
        summary_file.write(f"Memory: {mem_total}\n")
        summary_file.write(f"```\n")
        summary_file.write(f"\n")
        summary_file.write(f"## Log Files\n")
        summary_file.write(f"- Test Log: {LOG_FILE}\n")
        summary_file.write(f"- Server Log: server.log\n")
        summary_file.write(f"- Summary File: {SUMMARY_FILE}\n")
        summary_file.write(f"\n")
        if os.path.exists("error_summary.txt"):
            summary_file.write(f"## Error Summary\n")
            summary_file.write(f"```\n")
            with open("error_summary.txt") as error_file:
                summary_file.write(error_file.read())
            summary_file.write(f"```\n")

def init_log():
    with open(LOG_FILE, 'w') as log_file:
        log_file.write("========================================\n")
        log_file.write(f"IRC Server Test Log - {datetime.now(timezone.utc):%Y-%m-%d %H:%M:%S UTC}\n")
        log_file.write(f"User: {os.getlogin()}\n")
        log_file.write(f"Server: localhost:{PORT}\n")
        log_file.write(f"Debug Mode: {DEBUG}\n")
        log_file.write(f"Verbose Mode: {VERBOSE}\n")
        log_file.write("========================================\n")
        log_file.write("\n")

def print_separator():
    if not QUIET:
        print(f"{GRAY}----------------------------------------{NC}")
    with open(LOG_FILE, 'a') as log_file:
        log_file.write("----------------------------------------\n")

def log_message(message):
    timestamp = datetime.now(timezone.utc).strftime('%Y-%m-%d %H:%M:%S UTC')
    with open(LOG_FILE, 'a') as log_file:
        log_file.write(f"[{timestamp}] {message}\n")

def start_server():
    if not os.path.exists(SERVER_BINARY):
        print(f"{RED}Error: Server binary not found at {SERVER_BINARY}{NC}", file=sys.stderr)
        log_message(f"ERROR: Server binary not found at {SERVER_BINARY}")
        sys.exit(1)

    if not QUIET:
        print(f"\n{YELLOW}Starting IRC server...{NC}")
    log_message("Starting IRC server")
    server_process = subprocess.Popen([SERVER_BINARY, str(PORT), PASSWORD], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    time.sleep(2)  # Wait for server to initialize
    debug_msg(f"Waiting for server to start (PID: {server_process.pid})")

    if server_process.poll() is not None:
        print(f"{RED}Error: Server failed to start{NC}", file=sys.stderr)
        log_message("ERROR: Server failed to start")
        sys.exit(1)

    if not QUIET:
        print(f"{GREEN}Server started successfully{NC}")
    log_message("Server started successfully")
    return server_process

def debug_msg(message):
    if DEBUG:
        print(f"{GRAY}[DEBUG] {message}{NC}", file=sys.stderr)
        log_message(f"[DEBUG] {message}")

def load_custom_tests():
    if CUSTOM_TESTS:
        if not os.path.exists(CUSTOM_TESTS):
            print(f"{RED}Error: Custom test file not found: {CUSTOM_TESTS}{NC}", file=sys.stderr)
            log_message(f"ERROR: Custom test file not found: {CUSTOM_TESTS}")
            sys.exit(1)

        debug_msg(f"Loading custom tests from: {CUSTOM_TESTS}")
        log_message(f"Loading custom tests from: {CUSTOM_TESTS}")

        if not os.access(CUSTOM_TESTS, os.R_OK):
            print(f"{RED}Error: Cannot read test file: {CUSTOM_TESTS}{NC}", file=sys.stderr)
            log_message(f"ERROR: Cannot read test file: {CUSTOM_TESTS}")
            sys.exit(1)


def execute_tests():
    global TEST_START_TIME, TEST_END_TIME, TOTAL_TESTS, PASSED_TESTS, FAILED_TESTS, FAILED_TESTS_ARRAY
    TEST_START_TIME = datetime.now(timezone.utc)
    FAILED_TESTS_ARRAY = []

    if not CUSTOM_TESTS or not os.path.exists(CUSTOM_TESTS):
        print(f"{RED}Error: Test file not found: {CUSTOM_TESTS}{NC}", file=sys.stderr)
        return 1

    with open(CUSTOM_TESTS, 'r') as test_file:
        try:
            tests = yaml.safe_load(test_file)
        except yaml.YAMLError as exc:
            print(f"{RED}Error: Failed to parse YAML file: {exc}{NC}", file=sys.stderr)
            return 1

        for test in tests:
            TOTAL_TESTS += 1

            if 'name' not in test or 'commands' not in test or 'expect' not in test:
                print(f"{RED}Error: Invalid test format in test: {test}{NC}", file=sys.stderr)
                FAILED_TESTS += 1
                FAILED_TESTS_ARRAY.append(f"❌ Invalid test format in test: {test}")
                continue

            test_name = test['name']
            expected_response = test['expect']
            commands = '\r\n'.join(test['commands'])

            if run_single_test(test_name, expected_response, commands):
                PASSED_TESTS += 1
            else:
                FAILED_TESTS += 1
                FAILED_TESTS_ARRAY.append(f"❌ {test_name} - Expected: '{expected_response}'")

    TEST_END_TIME = datetime.now(timezone.utc)
    generate_summary()

    if not QUIET:
        print(f"\n{GREEN}Test Summary:{NC}")
        print(f"Total: {TOTAL_TESTS}")
        print(f"Passed: {GREEN}{PASSED_TESTS}{NC}")
        print(f"Failed: {RED}{FAILED_TESTS}{NC}")
        print(f"Summary written to: {BLUE}{SUMMARY_FILE}{NC}")

def print_short_summary():
    global TEST_END_TIME
    if not QUIET:
        if TEST_END_TIME is None:
            TEST_END_TIME = datetime.now(timezone.utc)
        duration = int(TEST_END_TIME.timestamp() - TEST_START_TIME.timestamp())
        print(f"\n{BOLD}Test Execution Summary:{NC}")
        print(f"{GRAY}----------------------------------------{NC}")
        print(f"Total Tests:    {BLUE}{TOTAL_TESTS}{NC}")
        print(f"Passed:         {GREEN}{PASSED_TESTS}{NC}")
        print(f"Failed:         {RED}{FAILED_TESTS}{NC}")
        if TOTAL_TESTS - SKIPPED_TESTS > 0:
            print(f"Success Rate:   {BLUE}{PASSED_TESTS * 100 / (TOTAL_TESTS - SKIPPED_TESTS):.2f}%{NC}")
        print(f"Duration:       {BLUE}{duration} seconds{NC}")
        print(f"{GRAY}----------------------------------------{NC}")
        print(f"Detailed summary: {BLUE}{SUMMARY_FILE}{NC}")

def run_single_test(test_name, expected, commands):
    if not QUIET:
        print(f"\n{BOLD}Running: {test_name}{NC}")
    log_message(f"Running test: {test_name}")

    if send_and_receive(commands, expected):
        print(f"{OK} Test passed: {test_name}")
        PASSED_TESTS += 1
        return True
    else:
        print(f"{KO} Test failed: {test_name}")
        FAILED_TESTS += 1
        return False

def send_and_receive(commands, expected):
    debug_msg(f"Sending commands: {commands}")
    
    with tempfile.NamedTemporaryFile() as tmpfile:
        nc_process = subprocess.Popen(['nc', 'localhost', str(PORT)], stdin=subprocess.PIPE, stdout=tmpfile, stderr=subprocess.PIPE)
        nc_process.communicate(input=commands.encode())

        response = tmpfile.read().decode()

    if VERBOSE:
        print(f"{GRAY}Server Response:{NC}\n{response}")

    return expected in response

def cleanup(server_process):
    if not SKIP_CLEANUP and server_process:
        log_message(f"Stopping server (PID: {server_process.pid})")
        server_process.terminate()
        server_process.wait()

def show_config():
    if not QUIET:
        print(f"\n{BOLD}Current Configuration:{NC}")
        print(f"{GRAY}----------------------------------------{NC}")
        print(f"Server Port:        {BLUE}{PORT}{NC}")
        print(f"Server Binary:      {BLUE}{SERVER_BINARY}{NC}")
        print(f"Timeout:           {BLUE}{TIMEOUT}{NC} seconds")
        print(f"Log File:          {BLUE}{LOG_FILE}{NC}")
        print(f"Debug Mode:        {BLUE}{'Enabled' if DEBUG else 'Disabled'}{NC}")
        print(f"Verbose Mode:      {BLUE}{'Enabled' if VERBOSE else 'Disabled'}{NC}")
        print(f"Color Output:      {BLUE}{'Enabled' if not NO_COLOR else 'Disabled'}{NC}")
        print(f"Custom Tests:      {BLUE}{CUSTOM_TESTS if CUSTOM_TESTS else 'None'}{NC}")
        print(f"Skip Cleanup:      {BLUE}{'Yes' if SKIP_CLEANUP else 'No'}{NC}")
        print(f"{GRAY}----------------------------------------{NC}\n")

def show_usage():
    print(f"""
Usage: {sys.argv[0]} [OPTIONS]

Enhanced IRC Server Test Suite

Options:
    -h, --help                Show this help message
    -d, --debug              Enable debug mode with detailed logging
    -p, --port PORT          Specify server port (default: {DEFAULT_PORT})
    -t, --timeout SEC        Set command timeout (default: {DEFAULT_TIMEOUT})
    -l, --log FILE          Specify log file (default: {DEFAULT_LOG_FILE})
    -v, --verbose           Enable verbose output
    -q, --quiet             Suppress all non-error output
    -n, --no-color          Disable colored output
    -f, --test-file FILE    Run custom tests from file
    -s, --skip-cleanup      Don't cleanup server process on exit

Examples:
    {sys.argv[0]} -d -p 6667           (Using short options)
    {sys.argv[0]} --verbose --timeout 10
    {sys.argv[0]} -f custom_tests.txt -v
    {sys.argv[0]} -t 5 -p 22200 -v

Report bugs to: your@email.com
""")
    sys.exit(0)

def main():
    global PORT, PASSWORD, SERVER_BINARY, TIMEOUT, LOG_FILE, MAX_PARALLEL_TESTS
    global DEBUG, VERBOSE, QUIET, NO_COLOR, SKIP_CLEANUP, CUSTOM_TESTS

    parser = argparse.ArgumentParser(description="Enhanced IRC Server Test Suite")
    parser.add_argument('-d', '--debug', action='store_true', help='Enable debug mode with detailed logging')
    parser.add_argument('-p', '--port', type=int, help=f'Specify server port (default: {DEFAULT_PORT})')
    parser.add_argument('-t', '--timeout', type=int, help=f'Set command timeout (default: {DEFAULT_TIMEOUT})')
    parser.add_argument('-l', '--log', help=f'Specify log file (default: {DEFAULT_LOG_FILE})')
    parser.add_argument('-v', '--verbose', action='store_true', help='Enable verbose output')
    parser.add_argument('-q', '--quiet', action='store_true', help='Suppress all non-error output')
    parser.add_argument('-n', '--no-color', action='store_true', help='Disable colored output')
    parser.add_argument('-f', '--test-file', help='Run custom tests from file')
    parser.add_argument('-s', '--skip-cleanup', action='store_true', help="Don't cleanup server process on exit")
    args = parser.parse_args()

    if args.debug:
        DEBUG = True
    if args.port:
        PORT = args.port
    if args.timeout:
        TIMEOUT = args.timeout
    if args.log:
        LOG_FILE = args.log
    if args.verbose:
        VERBOSE = True
    if args.quiet:
        QUIET = True
    if args.no_color:
        NO_COLOR = True
        global RED, GREEN, YELLOW, BLUE, GRAY, NC, BOLD, OK, KO, INFO, WARN
        RED = GREEN = YELLOW = BLUE = GRAY = NC = BOLD = ""
        OK = "[OK]"
        KO = "[KO]"
        INFO = "[INFO]"
        WARN = "[WARN]"
    if args.test_file:
        CUSTOM_TESTS = args.test_file
    if args.skip_cleanup:
        SKIP_CLEANUP = True

    init_log()
    show_config()

    if not QUIET:
        print(f"{BLUE}{BOLD}Enhanced IRC Server Test Suite{NC}")
        print(f"{GRAY}Current Date and Time (UTC): {datetime.now(timezone.utc):%Y-%m-%d %H:%M:%S}{NC}")
        print(f"{GRAY}Current User's Login: {os.getlogin()}{NC}")
        print(f"{GRAY}Testing server at localhost:{PORT}{NC}")

    log_message("Test suite started")
    print_separator()

    server_process = start_server()
    load_custom_tests()
    execute_tests()
    print_short_summary()

    if not QUIET:
        print(f"\n{YELLOW}Stopping server...{NC}")
    log_message("Test suite shutting down")
    cleanup(server_process)

    print_separator()
    if not QUIET:
        print(f"{GREEN}Tests completed successfully{NC}")
        print(f"{GRAY}Log file: {LOG_FILE}{NC}")
        print(f"{GRAY}Server log: server.log{NC}")

if __name__ == "__main__":
    main()
