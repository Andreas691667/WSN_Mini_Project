import socket
import os

LOGS_DIR = os.path.join("..", "logs")  # Path to logs directory from data_collection folder

# Connect to the Cooja serial socket
mote_sockets = []
for i in range(100):
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    try:
        sock.connect(("localhost", 60001 + i))
        sock.setblocking(0)
        mote_sockets.append(sock)
        print(f"Connected to mote {i+1}")
    except ConnectionRefusedError:
        continue  # Skip if the connection is refused

data_index = 1
# Check for existing data directories inside logs
while os.path.exists(os.path.join(LOGS_DIR, f"data_{data_index}")):
    data_index += 1

# Create the new data directory inside logs
new_data_directory = os.path.join(LOGS_DIR, f"data_{data_index}")
os.makedirs(new_data_directory)

# Read data from the mote's serial output
while True:
    for i, sock in enumerate(mote_sockets):
        try:
            data = sock.recv(1024)
            if data:
                file_path = os.path.join(new_data_directory, f"mote_{i}_data.txt")
                with open(file_path, "a") as f:
                    f.write(data.decode("utf-8"))
        except BlockingIOError:
            # No data available, move on
            continue