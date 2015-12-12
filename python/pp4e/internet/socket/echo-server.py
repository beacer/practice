#!/usr/bin/python3

from socket import *

myHost = '' # wildcard
myPort = 50007

sockobj = socket(AF_INET, SOCK_STREAM)
sockobj.bind((myHost, myPort))
sockobj.listen(3)

while True:
    conn, addr = sockobj.accept()
    print('Server connected by', addr)
    while True:
        data = conn.recv(1024)
        if not data: break;
        conn.send(b"=>" + data)
    conn.close()
