import socket
import argparse
import uuid
from _thread import start_new_thread
import string

# Function for fibonacci calculation
def fibonacci(n):
    x = 0
    y = 1
    for i in range(n):
        temp = x
        x = y
        y = temp + x
    return x

# Function for factorial calculation
def factorial(n):
    result = 1
    for i in range(n):
        result *= (i + 1)
    return result

# Function for each client
def threaded(c, addr):
    # Receive client name
    clientName = c.recv(1024)
    print("Client name is: " + clientName.decode('utf-8'))

    # Send unique UUID to client
    ID = str(uuid.uuid3(uuid.NAMESPACE_URL, str(addr[0]) + ':' + str(addr[1])))
    encID = ID.encode('utf-8')
    print(c.sendall(encID))
    
    # Get request and send result
    while True:
        request = (c.recv(1024)).decode('utf-8')
        print(request)

        if request == 'FIBONACCI':
            N = (c.recv(1024)).decode('utf-8')
            result = fibonacci(int(N))
            msg = str(result) + "_" + ID
            c.send(msg.encode('utf-8'))

            print("[" + str(addr[0]) + ':' + str(addr[1]) + "] Fibonacci request from client, fib of " + str(result))

        elif request == 'FACTORIAL':
            N = (c.recv(1024)).decode('utf-8')
            result = factorial(int(N))
            msg = str(result) + "_" + ID
            c.send(msg.encode('utf-8'))

            print("[" + str(addr[0]) + ':' + str(addr[1]) + "] Factorial request from client, fib of " + str(result))

        elif request == 'FILE':
            file_len = int(c.recv(1024))

            file_con = c.recv(file_len)
            file_left = file_len - len(file_con)
            while file_left:
                file_con += c.recv(file_left)
                file_left = file_len - len(file_con)
            
            result = len(file_con.split(b" "))
            msg = str(result) + "_" + ID
            c.send(msg.encode('utf-8'))

            print("[" + str(addr[0]) + ':' + str(addr[1]) + "] File request from client, words in file are " + str(result))

        elif request == 'COMPLETE':
            break
    
    c.close()


def main(port):
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    host = '127.0.0.1'
    s.bind((host, port))
    s.listen(5)

    while True:
        c, addr = s.accept()
        print("Connected: " + str(addr[0]) + ':' + str(addr[1]))
        start_new_thread(threaded, (c, addr))
    s.close()
    

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description="python server.py --port=1234")
    parser.add_argument('--port', help="port number", required=True)
    args = parser.parse_args()
    main(port=int(args.port))
