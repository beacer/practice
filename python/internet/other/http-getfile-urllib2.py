#!/usr/bin/python3

import sys, os, urllib.request, urllib.parse

try:
    srvname, fname = sys.argv[1:3]
except:
    srvname, fname = 'learning-python.com', '/index.html'

url = 'http://%s%s' % (srvname, fname)

if len(sys.argv) == 4:
    localname = sys.argv[3]
else:
    (scheme, server, path, param, query, frag) = urllib.parse.urlparse(url)
    localname = os.path.split(path)[-1]

print(url, scheme, server, path)

urllib.request.urlretrieve(url, localname)
