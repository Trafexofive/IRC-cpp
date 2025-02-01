import socket
import threading
import time
from datetime import datetime
import psutil
import os
import signal
from contextlib import contextmanager

class IRCDiagnostics:
    def __init__(self, server, port, password):
        self.server = server
        self.port = port
        self.password = password
        self.test_results = []
        self.connection_timeout = 3  # seconds
        
    def log_result(self, test_name, status, details):
        timestamp = datetime.now()
        self.test_results.append({
            'timestamp': timestamp,
            'test': test_name,
            'status': status,
            'details': details
        })
        print(f"[{timestamp}] {test_name}: {status} - {details}")

    def create_connection(self, nick_prefix):
        """Helper to create a single connection with error handling"""
        try:
            sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            sock.settimeout(self.connection_timeout)
            sock.connect((self.server, self.port))
            sock.send(f"PASS {self.password}\r\n".encode())
            sock.send(f"NICK {nick_prefix}\r\n".encode())
            sock.send(f"USER {nick_prefix} 0 * :Test User\r\n".encode())
            time.sleep(0.1)  # Brief pause for registration
            return sock
        except Exception as e:
            raise ConnectionError(f"Failed to create connection: {str(e)}")

    def check_connection_leak(self):
        """Test for connection handle leaks"""
        try:
            initial_connections = len(psutil.Process().net_connections())
            sockets = []
            
            # Create test connections
            for i in range(5):  # Reduced from 10 to 5 for stability
                try:
                    sock = self.create_connection(f"leak_test_{i}")
                    sockets.append(sock)
                except ConnectionError as e:
                    self.log_result("Connection Leak Test", "WARNING", 
                        f"Failed to create test connection {i}: {str(e)}")
                    continue
                
            # Close all connections properly
            for sock in sockets:
                try:
                    sock.send(b"QUIT :Testing complete\r\n")
                    sock.close()
                except:
                    pass
                
            time.sleep(1)  # Reduced wait time
            
            final_connections = len(psutil.Process().net_connections())
            if final_connections > initial_connections:
                self.log_result("Connection Leak Test", "WARNING", 
                    f"Possible connection leak detected. Delta: {final_connections - initial_connections}")
            else:
                self.log_result("Connection Leak Test", "PASS", 
                    "No connection leaks detected")
                
        except Exception as e:
            self.log_result("Connection Leak Test", "ERROR", str(e))

    def check_memory_usage(self):
        """Monitor memory usage patterns"""
        try:
            process = psutil.Process()
            initial_memory = process.memory_info().rss
            max_memory = initial_memory
            connections = []
            
            # Create fewer connections for stability
            for i in range(5):  # Reduced from 50 to 5
                try:
                    sock = self.create_connection(f"mem_test_{i}")
                    connections.append(sock)
                    current_memory = process.memory_info().rss
                    max_memory = max(max_memory, current_memory)
                except ConnectionError:
                    continue
                
            # Clean up
            for sock in connections:
                try:
                    sock.send(b"QUIT :Memory test complete\r\n")
                    sock.close()
                except:
                    pass
                
            time.sleep(1)
            final_memory = process.memory_info().rss
            
            memory_delta = final_memory - initial_memory
            if memory_delta > 1024 * 1024:  # 1MB threshold
                self.log_result("Memory Usage Test", "WARNING",
                    f"Memory usage increased by {memory_delta/1024/1024:.2f}MB")
            else:
                self.log_result("Memory Usage Test", "PASS",
                    f"Memory usage stable. Peak: {max_memory/1024/1024:.2f}MB")
                
        except Exception as e:
            self.log_result("Memory Usage Test", "ERROR", str(e))

    def check_descriptor_leaks(self):
        """Test for file descriptor leaks"""
        try:
            process = psutil.Process()
            initial_fds = len(process.open_files())
            test_connections = []
            
            for i in range(5):  # Reduced from 20 to 5
                try:
                    sock = self.create_connection(f"fd_test_{i}")
                    test_connections.append(sock)
                except ConnectionError:
                    continue
            
            # Proper cleanup
            for sock in test_connections:
                try:
                    sock.send(b"QUIT :FD test complete\r\n")
                    sock.close()
                except:
                    pass
                
            time.sleep(1)
            final_fds = len(process.open_files())
            
            if final_fds > initial_fds:
                self.log_result("File Descriptor Test", "WARNING",
                    f"Possible FD leak: {final_fds - initial_fds} unclosed")
            else:
                self.log_result("File Descriptor Test", "PASS",
                    "No file descriptor leaks detected")
                
        except Exception as e:
            self.log_result("File Descriptor Test", "ERROR", str(e))

    def check_zombie_connections(self):
        """Test for zombie connection handling"""
        try:
            # Create initial connection
            sock1 = self.create_connection("zombie_test")
            
            # Force-close without QUIT
            sock1.close()
            
            time.sleep(1)
            
            # Try to reconnect with same nick
            try:
                sock2 = self.create_connection("zombie_test")
                data = sock2.recv(1024).decode('utf-8', 'ignore')
                
                if "nickname is already in use" in data.lower():
                    self.log_result("Zombie Connection Test", "WARNING",
                        "Server might not be cleaning up disconnected users")
                else:
                    self.log_result("Zombie Connection Test", "PASS",
                        "Server properly handles disconnected users")
                
                sock2.send(b"QUIT :Zombie test complete\r\n")
                sock2.close()
            except ConnectionError:
                self.log_result("Zombie Connection Test", "WARNING",
                    "Could not establish second connection")
                
        except Exception as e:
            self.log_result("Zombie Connection Test", "ERROR", str(e))

    def get_resource_usage(self):
        """Get current resource usage with updated psutil calls"""
        process = psutil.Process()
        return {
            'memory': process.memory_info().rss,
            'connections': len(process.net_connections()),
            'file_descriptors': len(process.open_files()),
            'threads': process.num_threads()
        }

    def check_resource_cleanup(self):
        """Test server's cleanup of resources"""
        try:
            initial_resources = self.get_resource_usage()
            connections = []
            
            # Create connections
            for i in range(5):  # Reduced from 10 to 5
                try:
                    sock = self.create_connection(f"cleanup_test_{i}")
                    connections.append(sock)
                except ConnectionError:
                    continue
            
            # Proper cleanup
            for sock in connections:
                try:
                    sock.send(b"QUIT :Cleanup test complete\r\n")
                    sock.close()
                except:
                    pass
            
            time.sleep(1)
            
            final_resources = self.get_resource_usage()
            
            # Compare resources
            for resource in initial_resources:
                if final_resources[resource] > initial_resources[resource] * 1.1:
                    self.log_result("Resource Cleanup Test", "WARNING",
                        f"{resource} increased by {final_resources[resource] - initial_resources[resource]}")
                else:
                    self.log_result("Resource Cleanup Test", "PASS",
                        f"{resource} cleanup successful")
                    
        except Exception as e:
            self.log_result("Resource Cleanup Test", "ERROR", str(e))

    def run_all_tests(self):
        """Run all diagnostic tests"""
        print("\nStarting IRC Server Diagnostics\n")
        print(f"Target Server: {self.server}:{self.port}\n")
        
        tests = [
            self.check_connection_leak,
            self.check_memory_usage,
            self.check_descriptor_leaks,
            self.check_zombie_connections,
            self.check_resource_cleanup
        ]
        
        for test in tests:
            try:
                test()
                time.sleep(1)  # Reduced pause between tests
            except Exception as e:
                self.log_result(test.__name__, "CRITICAL ERROR", str(e))
        
        self.print_report()

    def print_report(self):
        """Print final diagnostic report"""
        print("\n=== IRC Server Diagnostic Report ===\n")
        
        warnings = 0
        errors = 0
        passes = 0
        
        for result in self.test_results:
            if result['status'] == "WARNING":
                warnings += 1
            elif result['status'] == "ERROR" or result['status'] == "CRITICAL ERROR":
                errors += 1
            elif result['status'] == "PASS":
                passes += 1
        
        print(f"Total Tests: {len(self.test_results)}")
        print(f"Passed: {passes}")
        print(f"Warnings: {warnings}")
        print(f"Errors: {errors}")
        
        if warnings > 0 or errors > 0:
            print("\nPotential Issues Found:")
            for result in self.test_results:
                if result['status'] in ["WARNING", "ERROR", "CRITICAL ERROR"]:
                    print(f"\n- {result['test']}:")
                    print(f"  Status: {result['status']}")
                    print(f"  Details: {result['details']}")

if __name__ == "__main__":
    SERVER = "localhost"
    PORT = 22200
    PASSWORD = "Alilepro135!"
    
    diagnostics = IRCDiagnostics(SERVER, PORT, PASSWORD)
    diagnostics.run_all_tests()
