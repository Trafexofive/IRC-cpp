import socket
import threading
import time
from datetime import datetime
import random

# Test channels
PUBLIC_CHANNELS = ['#general', '#lobby', '#public']
PRIVATE_CHANNELS = ['#staff', '#admin', '#mods']

def join_channels(sock, client_id):
    """Join a mix of public and private channels"""
    try:
        # Join 1-2 public channels
        num_public = random.randint(1, 2)
        chosen_public = random.sample(PUBLIC_CHANNELS, num_public)
        for channel in chosen_public:
            sock.send(f"JOIN {channel}\r\n".encode())
            print(f"[{datetime.now()}] Client {client_id}: Sent join request for {channel}")
        
        # Join 0-1 private channels
        if random.random() < 0.3:
            channel = random.choice(PRIVATE_CHANNELS)
            sock.send(f"JOIN {channel}\r\n".encode())
            print(f"[{datetime.now()}] Client {client_id}: Sent join request for {channel}")
            
    except Exception as e:
        print(f"[{datetime.now()}] Client {client_id}: Failed to send join commands - {str(e)}")

def connect_client(server, port, client_id, test_duration=30):
    """Single client connection with timeout"""
    try:
        # Create socket with timeout
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.settimeout(5)  # 5 second timeout for operations
        sock.connect((server, port))
        
        # Authentication
        sock.send(f"PASS Alilepro135!\r\n".encode())
        sock.send(f"NICK loadtest_{client_id}\r\n".encode())
        sock.send(f"USER loadtest_{client_id} 0 * :Load Test Bot\r\n".encode())
        
        # Brief pause before joining
        time.sleep(0.5)
        
        # Join channels
        join_channels(sock, client_id)
        
        # Set start time for test duration
        start_time = time.time()
        
        # Keep connection alive for test_duration seconds
        while time.time() - start_time < test_duration:
            try:
                sock.settimeout(1)  # Short timeout for reads
                data = sock.recv(1024)
                if data:
                    print(f"[{datetime.now()}] Client {client_id}: Received data: {data[:100]}")
            except socket.timeout:
                continue
            except Exception as e:
                print(f"[{datetime.now()}] Client {client_id}: Read error - {str(e)}")
                break
                
    except Exception as e:
        print(f"[{datetime.now()}] Client {client_id}: Connection error - {str(e)}")
    finally:
        try:
            sock.close()
            print(f"[{datetime.now()}] Client {client_id}: Connection closed")
        except:
            pass

def run_channel_test(server, port, num_clients, test_duration=30):
    """Main test function with duration limit"""
    print(f"Starting channel load test with {num_clients} clients")
    print(f"Server: {server}:{port}")
    print(f"Test duration: {test_duration} seconds")
    print("Public channels:", ', '.join(PUBLIC_CHANNELS))
    print("Private channels:", ', '.join(PRIVATE_CHANNELS))
    
    threads = []
    
    # Create and start threads
    for i in range(num_clients):
        thread = threading.Thread(
            target=connect_client, 
            args=(server, port, i, test_duration)
        )
        thread.start()
        threads.append(thread)
        time.sleep(0.1)  # Small delay between client starts
    
    # Wait for all threads with timeout
    start_time = time.time()
    while time.time() - start_time < test_duration + 10:  # Extra 10 seconds for cleanup
        if not any(t.is_alive() for t in threads):
            break
        time.sleep(1)
    
    print("\nTest completed!")
    
    # Count still-running threads
    running = sum(1 for t in threads if t.is_alive())
    if running:
        print(f"Note: {running} clients still running (they will timeout shortly)")

if __name__ == "__main__":
    SERVER = "localhost"  # Change to your server address
    PORT = 22200
    NUM_CLIENTS = 30     # Number of simultaneous clients
    TEST_DURATION = 30   # How long to run the test in seconds
    
    run_channel_test(SERVER, PORT, NUM_CLIENTS, TEST_DURATION)
