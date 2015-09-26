#!/usr/bin/python3

import os, sys
from http.server import HTTPServer, CGIHTTPRequestHandler

webdir = "."
port = 8080

os.chdir(webdir)
srvaddr = ("", port)
srvobj = HTTPServer(srvaddr, CGIHTTPRequestHandler)
srvobj.serve_forever()
