#!/usr/bin/python3

import sys, signal, time

def now():
    return time.asctime()

def onSignal(signum, stackframe):
    print('Got signal', signum, 'at', now())
    if signum == signal.SIGCHLD:
        print('sigchld caught')

signum = int(sys.argv[1])
signal.signal(signum, onSignal)

while True:
    signal.pause()
