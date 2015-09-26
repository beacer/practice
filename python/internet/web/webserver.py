#!/usr/bin/python3

import os, sys
from http.server import HTTPServer, CGIHTTPRequestHandler

webdir = './www'
port = 80

if len(sys.argv) > 1: webdir = sys.argv[1]
if len(sys.argv) > 2: port = int(sys.argv[2])
print("Webdir %s, port %d" % (webdir, port))

os.chdir(webdir)
servaddr = ("", port)
serv = HTTPServer(servaddr, CGIHTTPRequestHandler)
serv.serve_forever()
