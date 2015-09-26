#!/usr/bin/python3

import os, time, sys

fifoname = "./tmpfifo"

def child():
    pipeout = os.open(fifoname, os.O_WRONLY)
    zzz = 0
    while True:
        time.sleep(zzz)
        msg = ('Spam %03d\n' % zzz).encode()
        os.write(pipeout, msg)
        zzz = (zzz + 1) % 5

def parent():
    pipein = open(fifoname, 'r')
    while True:
        line = pipein.readline()[:-1]
        if len(line) == 0:
            break # readline return '' for eof , if EOF and not break it will dead lock
                  # readline return '' and it's not EOF ? it could not happened.
        print('Parent %d get "%s" at %s' % (os.getpid(), line, time.time()))

if __name__ == '__main__':
    if not os.path.exists(fifoname):
        os.mkfifo(fifoname)
    if len(sys.argv) == 1:
        parent()
    else:
        child()
