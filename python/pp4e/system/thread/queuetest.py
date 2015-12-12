#!/usr/bin/python3

numconsumers = 2
numproducers = 4
nummessages  = 4

import threading, queue, time

safeprint = threading.Lock()
data_queue = queue.Queue()

def producer(idnum):
    for msgnum in range(nummessages):
        time.sleep(idnum)
        data_queue.put('[producer id=%d, count=%d]' % (idnum, msgnum))

def consumer(idnum):
    while True:
        time.sleep(0.1)
        try:
            data = data_queue.get(block = False)
        except queue.Empty:
            pass
        else:
            with safeprint:
                print('consumer', idnum, 'get =>', data)

if __name__ == '__main__':
    threads = []
    for i in range(numconsumers):
        thr = threading.Thread(target=consumer, args=(i,))
        thr.start()
        threads.append(thr)

    for i in range(numproducers):
        thr = threading.Thread(target=producer, args=(i,))
        thr.start()
        threads.append(thr)

    time.sleep(((numproducers - 1) * nummessages) + 1)
    print('main thread exit.')

    # actually we use threading module, even main thread 
    # exist other thread can still run

    # for thr in threads:
    #     thr.join()  # consumer never return

