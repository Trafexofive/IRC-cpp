import socket
import threading
import time
import random
from datetime import datetime

def connect_client(server, port, client_id):
    """Single client connection function with proper authentication"""
    try:
        # Create socket
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.connect((server, port))
        
        # Authenticate
        sock.send(f"PASS Alilepro135!\r\n".encode())
        sock.send(f"NICK loadtest_{client_id}\r\n".encode())
        sock.send(f"USER loadtest_{client_id} 0 * :Load Test Bot\r\n".encode())
        
        # Log successful connection
        print(f"[{datetime.now()}] Client {client_id}: Connected successfully")
        
        # Keep connection alive
        while True:
            try:
                data = sock.recv(1024)
                if not data:
                    break
                if b"PING" in data:
                    sock.send(b"PONG\r\n")
            except:
                break
                
    except Exception as e:
        print(f"[{datetime.now()}] Client {client_id}: Connection failed - {str(e)}")
    finally:
        try:
            sock.close()
        except:
            pass

def run_load_test(server, port, num_clients, ramp_up_time):
    """Main load test function"""
    print(f"Starting load test with {num_clients} clients")
    print(f"Server: {server}:{port}")
    print(f"Ramp up time: {ramp_up_time} seconds")
    
    threads = []
    
    # Create and start client threads gradually
    for i in range(num_clients):
        thread = threading.Thread(target=connect_client, args=(server, port, i))
        threads.append(thread)
        thread.start()
        
        # Calculate sleep time for gradual ramp-up
        sleep_time = ramp_up_time / num_clients
        time.sleep(sleep_time)
    
    # Wait for all threads to complete
    for thread in threads:
        thread.join()

if __name__ == "__main__":
    SERVER = "localhost"  # Change to your server address
    PORT = 22200
    NUM_CLIENTS = 50     # Number of simultaneous clients
    RAMP_UP_TIME = 30    # Time in seconds to gradually add all clients
    
    run_load_test(SERVER, PORT, NUM_CLIENTS, RAMP_UP_TIME)
