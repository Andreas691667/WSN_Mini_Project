import socket
import os

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
        continue  # Skip to the next socket if the connection is refused

data_index = 1
while os.path.exists(f"data_{data_index}"):
    data_index += 1
os.makedirs(f"data_{data_index}")

# Read data from the mote's serial output
while True:
    for i, sock in enumerate(mote_sockets):
        try:
            data = sock.recv(1024)
            if data:
                # print(data.decode("utf-8"))
                with open(f"data_{data_index}/mote_{i}_data.txt", "a") as f:
                    f.write(data.decode("utf-8"))
        except BlockingIOError:
            continue  # Skip to the next socket if no data is available
