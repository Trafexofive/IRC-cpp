import socket
import threading
import time
from datetime import datetime
import random
import resource
import os

def increase_limit():
    """Attempt to increase the file descriptor limit"""
    try:
        soft, hard = resource.getrlimit(resource.RLIMIT_NOFILE)
        # Try to set to hard limit
        resource.setrlimit(resource.RLIMIT_NOFILE, (hard, hard))
        print(f"Increased file descriptor limit from {soft} to {hard}")
        return hard
    except Exception as e:
        print(f"Could not increase file descriptor limit: {e}")
        return soft

class ConnectionPool:
    """Manages a pool of connections to prevent overwhelming system limits"""
    def __init__(self, max_connections=500):
        self.semaphore = threading.Semaphore(max_connections)
        self.active_connections = 0
        self.lock = threading.Lock()

    def acquire(self):
        return self.semaphore.acquire(timeout=5)  # 5 second timeout

    def release(self):
        self.semaphore.release()

def join_channels(sock, client_id):
    """Join a mix of public and private channels"""
    try:
        # Join 1-2 public channels
        num_public = random.randint(1, 2)
        chosen_public = random.sample(['#general', '#lobby', '#public'], num_public)
        for channel in chosen_public:
            sock.send(f"JOIN {channel}\r\n".encode())
            print(f"[{datetime.now()}] Client {client_id}: Sent join request for {channel}")
        
        if random.random() < 0.3:
            channel = random.choice(['#staff', '#admin', '#mods'])
            passwd = random.randint(1, 1000)
            sock.send(f"JOIN {channel} {passwd}\r\n".encode())
            print(f"[{datetime.now()}] Client {client_id}: Sent join request for {channel}")
    except Exception as e:
        print(f"[{datetime.now()}] Client {client_id}: Failed to send join commands - {str(e)}")

def connect_client(server, port, client_id, pool, test_duration=30):
    """Single client connection with resource management"""
    if not pool.acquire():
        print(f"[{datetime.now()}] Client {client_id}: Could not acquire connection slot")
        return

    sock = None
    try:
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.settimeout(5)
        sock.connect((server, port))
        
        sock.send(f"PASS Alilepro135!\r\n".encode())
        sock.send(f"NICK loadtest_{client_id}\r\n".encode())
        sock.send(f"USER loadtest_{client_id} 0 * :Load Test Bot\r\n".encode())
        
        time.sleep(0.5)
        join_channels(sock, client_id)
        
        start_time = time.time()
        while time.time() - start_time < test_duration:
            try:
                sock.settimeout(1)
                data = sock.recv(1024)
                if data:
                    print(f"[{datetime.now()}] Client {client_id}: Received data")
            except socket.timeout:
                continue
            except Exception as e:
                print(f"[{datetime.now()}] Client {client_id}: Read error - {str(e)}")
                break
                
    except Exception as e:
        print(f"[{datetime.now()}] Client {client_id}: Connection error - {str(e)}")
    finally:
        if sock:
            try:
                sock.close()
            except:
                pass
        pool.release()
        print(f"[{datetime.now()}] Client {client_id}: Connection closed")

def run_channel_test(server, port, num_clients, test_duration=30):
    """Main test function with connection pooling"""
    # Get system limits
    max_fds = increase_limit()
    # Use 75% of available file descriptors, leaving some for system
    safe_connections = min(int(max_fds * 0.75), num_clients)
    
    print(f"Starting channel load test")
    print(f"Requested clients: {num_clients}")
    print(f"Safe connection limit: {safe_connections}")
    print(f"Server: {server}:{port}")
    print(f"Test duration: {test_duration} seconds")
    
    # Create connection pool
    pool = ConnectionPool(safe_connections)
    
    threads = []
    for i in range(num_clients):
        thread = threading.Thread(
            target=connect_client, 
            args=(server, port, i, pool, test_duration)
        )
        thread.start()
        threads.append(thread)
        time.sleep(0.1)
    
    # Wait for completion
    start_time = time.time()
    while time.time() - start_time < test_duration + 10:
        if not any(t.is_alive() for t in threads):
            break
        time.sleep(1)
    
    print("\nTest completed!")
    running = sum(1 for t in threads if t.is_alive())
    if running:
        print(f"Note: {running} clients still running (they will timeout shortly)")

if __name__ == "__main__":
    SERVER = "localhost"
    PORT = 22200
    NUM_CLIENTS = 11000    # This will be automatically limited to a safe number
    TEST_DURATION = 111030
    
    run_channel_test(SERVER, PORT, NUM_CLIENTS, TEST_DURATION)
