#!/usr/bin/python3

import sys, http.client

try:
    srvname, fname = sys.argv[1:]
except:
    srvname, fname = 'learning-python.com', '/index.html'

print(srvname, fname)

serv = http.client.HTTPConnection(srvname)
serv.putrequest('GET', fname)
serv.putheader('Accept', 'text/html')
serv.endheaders()

reply = serv.getresponse()
if reply.status != 200:
    print('Error:', reply.status, reply.reason)

else:
    data = reply.readlines()
reply.close()

for line in data:
    print(line)


