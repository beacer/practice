#!/usr/bin/python3

import os, time, sys, signal

from socket import *

myHost = ''
myPort = 50007

sockobj = socket(AF_INET, SOCK_STREAM)
sockobj.setsockopt(SOL_SOCKET, SO_REUSEADDR, 1)
sockobj.bind((myHost, myPort))
sockobj.listen(5)

# python only, and at least for OS X/Linux not all platfrom,
# if ignore SIGCHLD, parent didn't need to wait() child processes, 
# system (python) will handle the zombie.
# it's not POSIX behavior but python's (for some platform).
# POSIX always need wait()
signal.signal(signal.SIGCHLD, signal.SIG_IGN)

while True:
    # try:
        conn, addr = sockobj.accept()
    # except: # hadle interupted system call
    #     continue
    print('Receive connection', addr)

    pid = os.fork()
    if pid == 0: # client
        sockobj.close()
        while True:
            data = conn.recv(1024)
            if not data: break
            conn.send(data)
        conn.close()
        sys.exit()
    else:
        conn.close()
