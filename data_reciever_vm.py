import socket

# Connect to the Cooja serial socket
mote_sockets = []
for i in range(100):
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    try:
        sock.connect(('localhost', 60001 + i))
        sock.setblocking(0)
        mote_sockets.append(sock)
    except ConnectionRefusedError:
        continue  # Skip to the next socket if the connection is refused

# Read data from the mote's serial output
while True:
    for i, sock in enumerate(mote_sockets):
        try:
            data = sock.recv(1024)
            if data:
                print(data.decode('utf-8'))
                with open(f'mote_{i}_data.txt', 'a') as f:
                    f.write(data.decode('utf-8'))
        except BlockingIOError:
            continue  # Skip to the next socket if no data is available
