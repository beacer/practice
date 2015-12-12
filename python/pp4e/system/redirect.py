#!/usr/bin/python3

import sys

# this class support write() method, 
# so it can be used for sys.stdout
class Output:
    def __init__(self):
        self.text =''
    def write(self, string):
        self.text += string
    def writelines(self, lines):
        #self.text.append(lines)
        for l in lines:
            self.write(l)

class Input:
    def __init__(self, input=''):
        self.text = input
    def read(self, size=None):
        if size == None:
            res, self.text = self.text, ''
        else:
            res, self.text = self.text[:size], size.text[size:]
        return res
    def readline(size):
        eol = self.find('\n')
        if eol == -1:
            eol = None
        self.read(eol + 1)

def redirect(function, pargs, kargs, input):
    savestreams = [sys.stdin, stdout]
    sys.stdin = Input(input)
    sys.stdout = Output()

    try:
        result = function(*pargs, **kargs)
        output = sys.stdout.text
    finally:
        sys.stdin, sys.stdout = savestreams
    return (result, output)
