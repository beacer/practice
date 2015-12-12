#!/usr/bin/python3

import os, time, sys

from socket import *

myHost = ''
myPort = 50007

sockobj = socket(AF_INET, SOCK_STREAM)
sockobj.setsockopt(SOL_SOCKET, SO_REUSEADDR, 1)
sockobj.bind((myHost, myPort))
sockobj.listen(5)

# it's the better to use SIGCHLD to handle zombie
activeChildren = []
def reapChildren():
    while activeChildren:
        try:
            pid, stat = os.waitpid(0, os.WNOHANG)
        except ChildProcessError:
            pass
        else:
            if not pid: break;
            activeChildren.remove(pid)

while True:
    conn, addr = sockobj.accept()
    print('Receive connection', addr)

    pid = os.fork()
    if pid == 0: # client
        sockobj.close()
        while True:
            print("1")
            data = conn.recv(1024)
            if not data: break
            print("2")
            conn.send(data)
        conn.close()
        sys.exit()
    else:
        conn.close()
        activeChildren.append(pid)
        reapChildren()
