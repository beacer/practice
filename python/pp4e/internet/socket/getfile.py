#!/usr/bin/python3

import sys, os, time, threading
from socket import *

blksz = 1024
defhost = 'localhost'
defport = 50001

helptext = """
Usage:
    getfile.py -mode server [-port PORT] [-host HOST]
    getfile.py [-mode client] -file FILE [-port PORT] [-host HOST]
"""

def now():
    return time.asctime()

def parse_args():
    result = {}
    args = sys.argv[1:]
    while len(args) >= 2:
        result[args[0]] = args[1]
        args = args[2:]
    return result

def client(host, port, filename):
    sock = socket(AF_INET, SOCK_STREAM)
    sock.connect((host, port))
    sock.send((filename + '\n').encode())
    fname = os.path.split(filename)[1]
    file = open(fname, 'wb')
    while True:
        data = sock.recv(blksz)
        if not data: break
        file.write(data)
    file.close()
    sock.close()
    print('Client got', filename, 'at', now())


def server_thr(clisock):
    sockfile = clisock.makefile('r')
    fname = sockfile.readline()[:-1] # strip ending \n
    try:
        file = open(fname, 'rb')
        while True:
            bytes = file.read(blksz)
            if not bytes: break
            n = clisock.send(bytes)
            assert n == len(bytes)
    except:
        print('Error dowloading file on server:', fname)
    clisock.close()

def server(host, port):
    sock = socket(AF_INET, SOCK_STREAM)
    sock.setsockopt(SOL_SOCKET, SO_REUSEADDR, 1)
    sock.bind((host, port))
    sock.listen(5)
    while True:
        conn, addr = sock.accept()
        print('New connection:', addr)
        thr = threading.Thread(target=server_thr, args=(conn,))
        thr.start()

def main(args):
    host = args.get('-host', defhost)
    port = int(args.get('-port', defport))
    if args.get('-mode') == 'server':
        if host == 'localhost': host = ''
        server(host, port)
    elif args.get('-file'):
        client(host, port, args['-file'])
    else:
        print(helptext)

if __name__ == '__main__':
    args = parse_args()
    main(args)
