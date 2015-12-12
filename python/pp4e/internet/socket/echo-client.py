#!/usr/bin/python3

from socket import *

server = 'localhost'
port = 50007

s = socket(AF_INET, SOCK_STREAM)
s.connect((server, port))

while True:
    try:
        data = input('input> ')
    except EOFError:
        break;
    if not data: break;
    s.send(data.encode())
    reply = s.recv(1024)
    print(reply.decode())
    
s.close()
