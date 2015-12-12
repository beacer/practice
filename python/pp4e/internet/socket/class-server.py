#!/usr/bin/python3

import socketserver, time

myHost = ''
myPort = 50007

def now():
    print(time.asctime())

class MyClientHandler(socketserver.BaseRequestHandler):
    def handle(self):
        print(self.client_address, now())
        while True:
            data = self.request.recv(1024)
            if not data: break
            self.request.send(b'=>' + data)
        self.request.close()

server = socketserver.ThreadingTCPServer((myHost, myPort), MyClientHandler)
server.serve_forever()
