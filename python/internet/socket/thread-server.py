#!/usr/bin/python3

import os, time, sys, threading

from socket import *

myHost = ''
myPort = 50007

sockobj = socket(AF_INET, SOCK_STREAM)
sockobj.setsockopt(SOL_SOCKET, SO_REUSEADDR, 1)
sockobj.bind((myHost, myPort))
sockobj.listen(5)

def handle_client(conn):
    while True:
        data = conn.recv(1024)
        if not data: break
        conn.send(data)
    conn.close()

while True:
    conn, addr = sockobj.accept()
    print('Receive connection', addr)
    threading.Thread(target=handle_client, args=(conn, )).start()
