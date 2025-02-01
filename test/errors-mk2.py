import socket
import threading
import time
from datetime import datetime
import psutil
import os
import signal
import argparse
from contextlib import contextmanager
import logging

class IRCDiagnostics:
    def __init__(self, server, port, password, max_connections=5):
        self.server = server
        self.port = port
        self.password = password
        self.max_connections = max_connections
        self.test_results = []
        self.connection_timeout = 5  # Increased timeout
        self.server_process = None
        self.logger = self.setup_logger()

        # Find server process during initialization
        self.initialize_server_process()

    def setup_logger(self):
        logger = logging.getLogger('IRCDiagnostics')
        logger.setLevel(logging.INFO)
        handler = logging.StreamHandler()
        formatter = logging.Formatter('%(asctime)s - %(levelname)s - %(message)s')
        handler.setFormatter(formatter)
        logger.addHandler(handler)
        return logger

    def initialize_server_process(self):
        """Find the server process listening on the target port"""
        try:
            for conn in psutil.net_connections():
                if (conn.status == 'LISTEN' and 
                    conn.laddr.port == self.port and 
                    (self.server in ['localhost', '127.0.0.1', conn.laddr.ip])):
                    try:
                        self.server_process = psutil.Process(conn.pid)
                        self.logger.info(f"Found server process: PID {conn.pid}")
                        return
                    except psutil.NoSuchProcess:
                        continue
            raise RuntimeError("Could not find server process")
        except Exception as e:
            self.logger.error(f"Server process discovery failed: {str(e)}")
            raise

    def get_server_resources(self):
        """Get resource usage for the server process"""
        if not self.server_process:
            self.initialize_server_process()
            
        try:
            with self.server_process.oneshot():
                return {
                    'memory': self.server_process.memory_info().rss,
                    'connections': len([
                        conn for conn in self.server_process.connections()
                        if conn.status == 'ESTABLISHED'
                    ]),
                    'fds': self.server_process.num_fds(),
                    'threads': self.server_process.num_threads()
                }
        except psutil.NoSuchProcess:
            self.initialize_server_process()
            return self.get_server_resources()

    @contextmanager
    def irc_connection(self, nick):
        """Context manager for IRC connections with proper cleanup"""
        sock = None
        try:
            sock = socket.create_connection((self.server, self.port), self.connection_timeout)
            sock.settimeout(self.connection_timeout)

            # Send registration commands
            sock.sendall(f"PASS {self.password}\r\n".encode())
            sock.sendall(f"NICK {nick}\r\n".encode())
            sock.sendall(f"USER {nick} 0 * :Diagnostic Client\r\n".encode())

            # Handle server PING
            start_time = time.time()
            while time.time() - start_time < self.connection_timeout:
                data = sock.recv(1024).decode('utf-8', 'ignore')
                if 'PING' in data:
                    ping_msg = data.split()[1]
                    sock.sendall(f"PONG {ping_msg}\r\n".encode())
                    break

            yield sock
        except Exception as e:
            self.logger.error(f"Connection error: {str(e)}")
            raise
        finally:
            if sock:
                try:
                    sock.sendall(b"QUIT :Diagnostic cleanup\r\n")
                    sock.close()
                except:
                    pass

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
        """Test for connection resource leaks on the server"""
        initial = self.get_server_resources()
        connections = []

        try:
            # Create multiple connections
            for i in range(self.max_connections):
                try:
                    with self.irc_connection(f"leak_test_{i}"):
                        connections.append(i)
                        time.sleep(0.2)
                except Exception as e:
                    self.log_result("Connection Leak", "WARNING", 
                                  f"Connection {i} failed: {str(e)}")

            # Allow time for server cleanup
            time.sleep(2)
            final = self.get_server_resources()

            # Calculate differences
            delta = final['connections'] - initial['connections']
            if delta > self.max_connections * 0.2:  # Allow 20% margin
                self.log_result("Connection Leak", "WARNING",
                               f"Connection increase: {delta} (Initial: {initial['connections']}, Final: {final['connections']})")
            else:
                self.log_result("Connection Leak", "PASS",
                               f"Connection count stable (Δ {delta})")

        except Exception as e:
            self.log_result("Connection Leak", "ERROR", str(e))

    def memory_leak_test(self):
        """Test for memory leaks during connection churn"""
        initial = self.get_server_resources()
        max_memory = initial['memory']

        try:
            connections = []
            for i in range(self.max_connections):
                try:
                    with self.irc_connection(f"mem_test_{i}"):
                        current = self.get_server_resources()
                        max_memory = max(max_memory, current['memory'])
                        time.sleep(0.5)
                except:
                    continue

            # Allow time for cleanup
            time.sleep(2)
            final = self.get_server_resources()

            # Evaluate results
            leak_size = final['memory'] - initial['memory']
            if leak_size > 5 * 1024 * 1024:  # 5MB threshold
                self.log_result("Memory Leak", "WARNING",
                               f"Memory increased by {leak_size/1024/1024:.2f}MB")
            else:
                self.log_result("Memory Leak", "PASS",
                               f"Memory stable (Δ {leak_size/1024/1024:.2f}MB)")

        except Exception as e:
            self.log_result("Memory Leak", "ERROR", str(e))

    def zombie_connection_test(self):
        """Test server's handling of abandoned connections"""
        try:
            # Create and abandon connection
            nick = "zombie_test"
            try:
                with self.irc_connection(nick) as sock:
                    sock.sendall(b"JOIN #test\r\n")
                    time.sleep(0.5)
            except:
                pass

            # Try to reuse nick
            time.sleep(2)
            try:
                with self.irc_connection(nick) as sock:
                    data = sock.recv(1024).decode('utf-8', 'ignore')
                    if "Nickname is already in use" in data:
                        self.log_result("Zombie Handling", "WARNING",
                                       "Server not reclaiming abandoned connections")
                    else:
                        self.log_result("Zombie Handling", "PASS",
                                       "Properly reclaimed abandoned connection")
            except Exception as e:
                self.log_result("Zombie Handling", "ERROR", str(e))

        except Exception as e:
            self.log_result("Zombie Handling", "ERROR", str(e))

    def run_all_tests(self):
        """Execute complete test suite"""
        tests = [
            self.connection_leak_test,
            self.memory_leak_test,
            self.zombie_connection_test
        ]

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
    parser.add_argument('--port', type=int, default=22200, help='IRC server port')
    parser.add_argument('--password', required=True, help='Server password')
    parser.add_argument('--connections', type=int, default=5, 
                       help='Max connections per test')

    args = parser.parse_args()

    try:
        diagnostics = IRCDiagnostics(
            server=args.server,
            port=args.port,
            password=args.password,
            max_connections=args.connections
        )
        diagnostics.run_all_tests()
    except Exception as e:
        print(f"Failed to initialize diagnostics: {str(e)}")
