#!/usr/bin/python3

import sys, select
from socket import *

myHost = ''
myPort = 50007
numPorts = 2

mainsocks, readsocks, writesocks = [], [], []
for i in range(numPorts):
    sock = socket(AF_INET, SOCK_STREAM)
    sock.setsockopt(SOL_SOCKET, SO_REUSEPORT, 1)
    sock.bind((myHost, myPort + i))
    sock.listen(5)
    mainsocks.append(sock)
    readsocks.append(sock)

while True:
    readables, writables, exceptions = select.select(readsocks, writesocks, [])
    for sock in readables:
        if sock in mainsocks:
            conn, addr = sock.accept()
            print('New connection:', addr)
            readsocks.append(conn)
        else:
            data = sock.recv(1024)
            if not data:
                sock.close()
                readsocks.remove(sock)
            else:
                # write may block, should use select for write too, 
                # but a buffer may needed
                sock.send(data)
