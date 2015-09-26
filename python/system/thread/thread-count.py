#!/usr/bin/python3

import _thread, time

def counter(myId, count):
    for i in range(count):
        time.sleep(1)
        print('[%s] =================> %s' % (myId, i))

for i in range(20):
    _thread.start_new_thread(counter, (i, 5))

time.sleep(6)
print('main thread exiting.')
