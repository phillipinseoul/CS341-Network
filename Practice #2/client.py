import argparse
import socket
import struct

# Function for checksum calculation
def checkSum(packet):
    s = 0
    n = len(packet) % 2
    for i in range(0, len(packet) - n, 2):
        w = packet[i] + (packet[i+1] << 8)
        temp = s + w
        s = (temp & 0xffff) + (temp >> 16)
    if n:
        s += packet[i+1]
    return ~s & 0xffff


def run(host, port, sid):

    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.connect((host, port))

        ### Step 1: Send Header ###

        # Create header
        FLAG = 1
        CHECK_SUM = 0
        KEYWORD = b"sbmt"
        SID = sid
        LENGTH = 16
        HEADER = struct.pack('!HH4sll', FLAG, CHECK_SUM, KEYWORD, SID, LENGTH)

        # Calculate checksum
        CHECK_SUM = checkSum(HEADER)

        # Insert checksum into header
        HEADER = struct.pack("!HH4sll", FLAG, CHECK_SUM, KEYWORD, SID, LENGTH)

        # Send header to server
        s.sendall(HEADER)


        ### Step 2: Receive Packets ###

        # Make lists to store headers & data
        HEAD_LIST = []
        WHOLE_DATA = b""

        # Receive packets from the server
        while True:
            # 1. Receive header
            recv_header = s.recv(16)
            unpk_header = struct.unpack('!HH4sll', recv_header)
            HEAD_LIST.append(unpk_header)
            FLAG_NUM = unpk_header[0]

            # 2. Receive data
            data_len = unpk_header[4] - 16
            recv_data = s.recv(data_len)
            data_left = data_len - len(recv_data)
            
            while data_left:
                recv_data += s.recv(data_left)
                data_left = data_len - len(recv_data)
            
            # 3. Store received data
            unpk_data = struct.unpack("!" + str(data_len) + 's', recv_data)
            WHOLE_DATA += unpk_data[0]

            if FLAG_NUM == 1:
                KEY = unpk_header[2]
                break

        ### Step 3: Send Packets ###

        # Decrypt data with XOR-Cipher
        k = 0
        decoded = b""
        for i in range(len(WHOLE_DATA)):
            A = WHOLE_DATA[i]
            B = KEY[k]
            decoded += chr(A ^ B).encode('utf-8')
            k += 1
            if k > 3:
                k = 0
        
        # Send decrypted packets to server
        KEYWORD = KEY
        FLAG = 0
        k = 0
        MAXLEN = 9984
        for index in range(0, len(decoded), MAXLEN):
            DATA = decoded[index : index + MAXLEN]
            CHECK_SUM = 0
            LENGTH = HEAD_LIST[k][4]
            k += 1

            if len(DATA) < MAXLEN:
                FLAG = 1

            form = '!HH4sll' + str(len(DATA)) + 's'
            PACKET = struct.pack(form, FLAG, CHECK_SUM, KEYWORD, SID, LENGTH, DATA)
            CHECK_SUM = checkSum(PACKET)
            PACKET = struct.pack(form, FLAG, CHECK_SUM, KEYWORD, SID, LENGTH, DATA)
            s.sendall(PACKET)

        # Receive final score
        final_msg = s.recv(1024)
        unpk_final = struct.unpack('!HH4sll' + str(len(final_msg) - 16) + 's', final_msg)
        print("KEY: " + unpk_final[2].decode('utf-8'))
        print("Score: " + str(unpk_final[3]))
        print("Message: " + unpk_final[5].decode('utf-8'))
    


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description="python client.py --host=143.248.56.39 --port=4000 --studentID=20xxxxxx")
    parser.add_argument('--host', help="host ip", required=True)
    parser.add_argument('--port', help="port number", required=True)
    parser.add_argument('--studentID', help="student id", required=True)

    args = parser.parse_args()
    run(host=args.host, port=int(args.port), sid=int(args.studentID))
