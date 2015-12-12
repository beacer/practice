#!/usr/bin/python3

import sys
from urllib.request import urlopen

try:
    srvname, fname = sys.argv[1:]
except:
    srvname, fname = 'learning-python.com', '/index.html'

url = 'http://%s%s' % (srvname, fname)
print(url)
rmfile = urlopen(url)
rmdata = rmfile.readlines()
rmfile.close()

for line in rmdata[:100]:
    print(line)
