from socket import *

left = []
right = []

def run_server():
    server_sock = socket(AF_INET, SOCK_STREAM)
    
    server_sock.bind(('', 80))
    server_sock.listen(5)
    print("Started")
    while(1):
        (client_sock, address) = server_sock.accept()
        client_sock.recv(500)
        data = client_sock.recv(4000)
        client_sock.send("ACK".encode())

        left.clear()
        right.clear()

        print(data)
        print(len(data)/4)
        for i in range(len(data) // 4):
            left.append(data[i+0] + data[i+1] * 256)
            right.append(data[i+2] + data[i+3] * 256)
        print("L:", left)
        print("R:", right)

    server_sock.close()

run_server()
