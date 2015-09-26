#!/usr/bin/python3

import sys
from socket import *

port = 50008
host = 'localhost'

def initListenerSocket(port=port):
    sock = socket(AF_INET, SOCK_STREAM)
    sock.setsockopt(SOL_SOCKET, SO_REUSEADDR, 1)
    sock.bind((host, port))
    sock.listen(5)
    conn, addr = sock.accept()
    return conn

def redirectOut(port=port, host=host):
    sock = socket(AF_INET, SOCK_STREAM)
    sock.connect((host, port))
    file = sock.makefile('w')
    sys.stdout = file
    return sock

def redirectIn(port=port, host=host):
    sock = socket(AF_INET, SOCK_STREAM)
    sock.connect((host, port))
    file = sock.makefile('r')
    sys.stdin = file
    return sock

def redirectBothAsClient(port=port, host=host):
    sock = socket(AF_INET, SOCK_STREAM)
    sock.connect((host, port))
    file = sock.makefile('rw')
    sys.stdin = file
    sys.stdout = file
    return sock
