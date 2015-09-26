#!/usr/bin/python3

import threading, time
count = 0

def adder(addlock):
    global count
    with addlock:
        count = count + 1
    time.sleep(0.5)
    with addlock:
        count = count + 1

addlock = threading.Lock()
threads = []
for i in range(100):
    thread = threading.Thread(target=adder, args=(addlock, ))
    thread.start()
    threads.append(thread)

for thread in threads:
    thread.join()

print(count)
