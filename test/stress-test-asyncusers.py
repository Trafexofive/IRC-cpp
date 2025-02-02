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
    """Synchronization barrier for coordinated connections"""
    def __init__(self, num_clients):
        self.barrier = threading.Barrier(num_clients + 1)  # +1 for main thread
        self.ready_lock = threading.Lock()
        self.ready_count = 0

    def wait_for_ready(self):
        with self.ready_lock:
            self.ready_count += 1
        self.barrier.wait()

def connect_client(server, port, client_id, barrier):
    """Single client with synchronized connection"""
    sock = None
    try:
        # Create socket but don't connect yet
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.settimeout(5)
        
        # Signal ready and wait for synchronized connect
        print(f"[{datetime.now()}] Client {client_id}: Ready for sync connect")
        barrier.wait_for_ready()
        
        # Synchronized connection attempt
        connect_time = time.time()
        sock.connect((server, port))
        connect_complete = time.time()
        connect_duration = (connect_complete - connect_time) * 1000  # ms
        print(f"[{datetime.now()}] Client {client_id}: Connected in {connect_duration:.2f}ms")
        
        # Quick authentication after connection
        sock.send(f"PASS Alilepro135!\r\n".encode())
        sock.send(f"NICK stress_{client_id}\r\n".encode())
        sock.send(f"USER stress_{client_id} 0 * :Stress Test\r\n".encode())
        
        # Brief wait to ensure registration completes
        try:
            data = sock.recv(1024).decode('utf-8', 'ignore')
            if data:
                print(f"[{datetime.now()}] Client {client_id}: Registration response received")
        except socket.timeout:
            print(f"[{datetime.now()}] Client {client_id}: No registration response (timeout)")
        
    except Exception as e:
        print(f"[{datetime.now()}] Client {client_id}: Connection failed - {str(e)}")
    finally:
        if sock:
            try:
                sock.close()
            except:
                pass

def run_sync_connect_test(server, port, num_clients):
    """Run synchronized connection test"""
    # Calculate safe client limit
    max_fds = increase_limit()
    safe_clients = min(int(max_fds * 0.75), num_clients)
    
    print(f"\nStarting synchronized connection test")
    print(f"Simultaneous connections: {safe_clients} (limited from {num_clients} based on system capacity)")
    print(f"Server: {server}:{port}")
    print("\nPreparing client threads...")
    
    # Create synchronization barrier
    barrier = SyncBarrier(safe_clients)
    threads = []
    
    # Create all client threads
    for i in range(safe_clients):
        thread = threading.Thread(
            target=connect_client,
            args=(server, port, i, barrier)
        )
        thread.start()
        threads.append(thread)
        time.sleep(0.05)  # Tiny delay for thread creation
    
    print("\nAll threads created, preparing for synchronized connect...")
    time.sleep(1)
    
    # Trigger synchronized connections
    print("\nExecuting synchronized connect...")
    start_time = time.time()
    barrier.wait_for_ready()
    
    # Wait for completion
    for thread in threads:
        thread.join(timeout=10)
    
    end_time = time.time()
    total_duration = (end_time - start_time) * 1000  # ms
    
    # Report results
    successful = sum(1 for t in threads if not t.is_alive())
    failed = sum(1 for t in threads if t.is_alive())
    
    print("\nTest Results:")
    print(f"Total duration: {total_duration:.2f}ms")
    print(f"Successful connections: {successful}")
    print(f"Failed/timeout connections: {failed}")
    
    if failed:
        print(f"\nNote: {failed} clients still running (they will timeout)")
    
    print("\nConnection stress test completed!")

if __name__ == "__main__":
    SERVER = "localhost"
    PORT = 22200
    NUM_CLIENTS = 300 # Will be automatically limited based on system capacity
    
    run_sync_connect_test(SERVER, PORT, NUM_CLIENTS)
