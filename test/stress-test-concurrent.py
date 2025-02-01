import socket
import threading
import time
from datetime import datetime
import random
import resource

def increase_limit():
    """Increase file descriptor limit temporarily"""
    try:
        soft, hard = resource.getrlimit(resource.RLIMIT_NOFILE)
        resource.setrlimit(resource.RLIMIT_NOFILE, (hard, hard))
        return hard
    except Exception as e:
        print(f"Could not increase file descriptor limit: {e}")
        return soft

class SyncBarrier:
    """Synchronization barrier for coordinated joins"""
    def __init__(self, num_clients):
        self.barrier = threading.Barrier(num_clients + 1)  # +1 for main thread
        self.ready_lock = threading.Lock()
        self.ready_count = 0

    def wait_for_ready(self):
        with self.ready_lock:
            self.ready_count += 1
        self.barrier.wait()

def connect_client(server, port, client_id, channel, barrier):
    """Client connection with synchronized join"""
    try:
        # Connect and authenticate
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.settimeout(5)
        sock.connect((server, port))
        
        # Basic registration
        sock.send(f"PASS Alilepro135!\r\n".encode())
        sock.send(f"NICK synctest_{client_id}\r\n".encode())
        sock.send(f"USER synctest_{client_id} 0 * :Sync Test Bot\r\n".encode())
        
        # Signal ready and wait for all clients
        print(f"[{datetime.now()}] Client {client_id}: Ready for sync join")
        barrier.wait_for_ready()
        
        # Synchronized join
        join_time = time.time()
        sock.send(f"JOIN {channel}\r\n".encode())
        print(f"[{datetime.now()}] Client {client_id}: Executed sync join")
        
        # Monitor join result
        try:
            data = sock.recv(1024).decode('utf-8', 'ignore')
            if "JOIN" in data:
                join_complete_time = time.time()
                join_duration = (join_complete_time - join_time) * 1000  # ms
                print(f"[{datetime.now()}] Client {client_id}: Join confirmed in {join_duration:.2f}ms")
        except socket.timeout:
            print(f"[{datetime.now()}] Client {client_id}: Join status unknown (timeout)")
        
        # Keep connection briefly to ensure join completes
        time.sleep(2)
        
    except Exception as e:
        print(f"[{datetime.now()}] Client {client_id}: Error - {str(e)}")
    finally:
        try:
            sock.close()
        except:
            pass

def run_sync_test(server, port, num_clients, channel):
    """Run synchronized join test"""
    max_fds = increase_limit()
    safe_clients = min(int(max_fds * 0.75), num_clients)
    
    print(f"\nStarting synchronized join test")
    print(f"Target channel: {channel}")
    print(f"Clients: {safe_clients} (limited from {num_clients} based on system capacity)")
    print(f"Server: {server}:{port}")
    print("\nInitializing clients...")
    
    barrier = SyncBarrier(safe_clients)
    threads = []
    
    # Create all client threads
    for i in range(safe_clients):
        thread = threading.Thread(
            target=connect_client,
            args=(server, port, i, channel, barrier)
        )
        thread.start()
        threads.append(thread)
        time.sleep(0.1)  # Small delay to prevent connection flood
    
    print("\nWaiting for all clients to be ready...")
    time.sleep(2)
    
    # Trigger synchronized join
    print("\nTriggering synchronized join...")
    barrier.wait_for_ready()
    
    # Wait for completion
    for thread in threads:
        thread.join(timeout=10)
    
    # Check for any hanging threads
    running = sum(1 for t in threads if t.is_alive())
    if running:
        print(f"\nNote: {running} clients still running (they will timeout)")
    
    print("\nSync join test completed!")

if __name__ == "__main__":
    SERVER = "localhost"
    PORT = 22200
    NUM_CLIENTS = 500     # Will be automatically limited based on system capacity
    TARGET_CHANNEL = "#general"
    
    run_sync_test(SERVER, PORT, NUM_CLIENTS, TARGET_CHANNEL)
