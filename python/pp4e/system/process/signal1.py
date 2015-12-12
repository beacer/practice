#!/usr/bin/python3

import sys, signal, time

def now():
    return time.ctime(time.time())

def onSignal(signum, stackframe):
    print('Got Signal', signum, 'at', now())

signum = int(sys.argv[1])
signal.signal(signum, onSignal)

while True:
    signal.pause()
