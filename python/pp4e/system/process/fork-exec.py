#!/usr/bin/python3

import os

param = 0
while True:
    param += 1
    pid = os.fork()
    if pid == 0:
        os.execlp('python', 'python', 'child.py', str(param))
        assert False, 'error starting program'
    else:
        print('child is', pid)
        if input() == 'q':
            break;

