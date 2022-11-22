import socket

sock = socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM)
sock.bind(("0.0.0.0", 12345))

while True:
    data, addr = sock.recvfrom(1024)
    print(f"Received {data} from {addr}")
