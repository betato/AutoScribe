#https://www.dj4e.com/code/http/server.py

from socket import *

def createServer():
    serversocket = socket(AF_INET, SOCK_STREAM)
    try :
        serversocket.bind(('',9000))
        serversocket.listen(5)
        while(1):
            (clientsocket, address) = serversocket.accept()
            print("AAAAA")
            rd = clientsocket.recv(5000).decode()
            print(rd)
            pieces = rd.split("\n")
            if ( len(pieces) > 0 ) : print(pieces[0])

    except KeyboardInterrupt :
        print("\nShutting down...\n");
    except Exception as exc :
        print("Error:\n");
        print(exc)

    serversocket.close()

print('Access http://localhost:9000')
createServer()
