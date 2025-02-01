import socket
import time
from datetime import datetime
import psutil
import logging
import argparse
from contextlib import contextmanager

class IRCDiagnostics:
    def __init__(self, server, port, password, max_connections=5, debug=False):
        self.server = server
        self.port = port
        self.password = password
        self.max_connections = max_connections
        self.debug = debug
        self.test_results = []
        self.connection_timeout = 10  # Increased timeout for unstable servers
        self.server_process = None
        self.logger = self.setup_logger()

    def setup_logger(self):
        """Configure logging with debug support"""
        logger = logging.getLogger('IRCDiagnostics')
        logger.setLevel(logging.DEBUG if self.debug else logging.INFO)
        handler = logging.StreamHandler()
        formatter = logging.Formatter('%(asctime)s - %(levelname)s - %(message)s')
        handler.setFormatter(formatter)
        logger.addHandler(handler)
        return logger

    @contextmanager
    def irc_connection(self, nick):
        """Context manager for IRC connections with robust error handling"""
        sock = None
        try:
            sock = socket.create_connection((self.server, self.port), self.connection_timeout)
            sock.settimeout(self.connection_timeout)

            # Send registration commands
            self._send_command(sock, f"PASS {self.password}")
            self._send_command(sock, f"NICK {nick}")
            self._send_command(sock, f"USER {nick} 0 * :Diagnostic Client")

            # Handle server PING (if implemented)
            self._handle_ping(sock)

            yield sock
        except Exception as e:
            self.logger.warning(f"Connection error for nick '{nick}': {str(e)}")
            raise
        finally:
            if sock:
                try:
                    self._send_command(sock, "QUIT :Diagnostic cleanup")
                    sock.close()
                except:
                    pass

    def _send_command(self, sock, command):
        """Helper to send commands with error handling"""
        try:
            sock.sendall(f"{command}\r\n".encode())
            time.sleep(0.1)  # Small delay to avoid overwhelming the server
        except Exception as e:
            self.logger.debug(f"Failed to send command '{command}': {str(e)}")
            raise

    def _handle_ping(self, sock):
        """Handle PING/PONG if the server implements it"""
        try:
            start_time = time.time()
            while time.time() - start_time < self.connection_timeout:
                data = sock.recv(1024).decode('utf-8', 'ignore')
                if data and 'PING' in data:
                    ping_msg = data.split()[1]
                    self._send_command(sock, f"PONG {ping_msg}")
                    break
        except Exception as e:
            self.logger.debug(f"PING/PONG handling failed: {str(e)}")

    def log_result(self, test_name, status, details):
        """Log test results with structured format"""
        result = {
            'timestamp': datetime.now(),
            'test': test_name,
            'status': status,
            'details': details
        }
        self.test_results.append(result)
        self.logger.log(
            logging.WARNING if status == 'WARNING' else logging.ERROR if 'ERROR' in status else logging.INFO,
            f"{test_name}: {status} - {details}"
        )

    def connection_leak_test(self):
        """Test for connection resource leaks"""
        try:
            initial_conns = self._get_server_connections()
            connections = []

            for i in range(self.max_connections):
                try:
                    with self.irc_connection(f"leak_test_{i}"):
                        connections.append(i)
                        time.sleep(0.5)
                except Exception as e:
                    self.log_result("Connection Leak", "WARNING", f"Connection {i} failed: {str(e)}")

            time.sleep(2)  # Allow server cleanup time
            final_conns = self._get_server_connections()

            delta = final_conns - initial_conns
            if delta > self.max_connections * 0.2:  # 20% tolerance
                self.log_result("Connection Leak", "WARNING", f"Connection increase: {delta}")
            else:
                self.log_result("Connection Leak", "PASS", f"Connection count stable (Δ {delta})")

        except Exception as e:
            self.log_result("Connection Leak", "ERROR", str(e))

    def _get_server_connections(self):
        """Get the number of active connections to the server"""
        try:
            return len([
                conn for conn in psutil.net_connections()
                if conn.status == 'ESTABLISHED' and
                conn.laddr.port == self.port and
                (self.server in ['localhost', '127.0.0.1', conn.laddr.ip])
            ])
        except Exception as e:
            self.logger.warning(f"Failed to get server connections: {str(e)}")
            return 0

    def run_test_suite(self, tests=None):
        """Run specified tests or all tests if none are specified"""
        if not tests:
            tests = [self.connection_leak_test]

        self.logger.info("Starting diagnostic tests...")
        for test in tests:
            try:
                test()
                time.sleep(1)
            except Exception as e:
                self.log_result(test.__name__, "CRITICAL ERROR", str(e))

        self.generate_report()

    def generate_report(self):
        """Generate final summary report"""
        self.logger.info("\n=== Diagnostic Report ===")
        stats = {'PASS': 0, 'WARNING': 0, 'ERROR': 0}

        for result in self.test_results:
            stats[result['status']] += 1

        self.logger.info(f"Tests Passed: {stats['PASS']}")
        self.logger.info(f"Warnings: {stats['WARNING']}")
        self.logger.info(f"Errors: {stats['ERROR']}")

        if stats['WARNING'] + stats['ERROR'] > 0:
            self.logger.info("\nDetailed Findings:")
            for result in self.test_results:
                if result['status'] != 'PASS':
                    self.logger.info(
                        f"{result['test']} [{result['status']}]: {result['details']}"
                    )

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='IRC Server Diagnostic Tool')
    parser.add_argument('--server', default='localhost', help='IRC server address')
    parser.add_argument('--port', type=int, default=6667, help='IRC server port')
    parser.add_argument('--password', required=True, help='Server password')
    parser.add_argument('--connections', type=int, default=5, help='Max connections per test')
    parser.add_argument('--debug', action='store_true', help='Enable debug logging')

    args = parser.parse_args()

    try:
        diagnostics = IRCDiagnostics(
            server=args.server,
            port=args.port,
            password=args.password,
            max_connections=args.connections,
            debug=args.debug
        )
        diagnostics.run_test_suite()
    except Exception as e:
        print(f"Failed to initialize diagnostics: {str(e)}")
