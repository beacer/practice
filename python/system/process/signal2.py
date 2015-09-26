#!/usr/bin/python3

import sys, signal, time

def now():
    return time.asctime()

def onSignal(signum, stackframe):
    print('Got alarm', signum, 'at', now())

while True:
    print('Setting at', now())
    signal.signal(signal.SIGALRM, onSignal)
    signal.alarm(5)
    signal.pause()


