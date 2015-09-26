#!/usr/bin/python3

import sys
from scanfile import scanner

class UnknowCommand(Exception):
    pass

commands = { '*':'Ms.', '+':'Mr.' }

def processLine(line):
    try:
        print(commands[line[0]], line[1:-1])
    except KeyError:
        raise UnknowCommand(line)

    # if line[0] in commands:
    #     print(line.replace(line[0], commands[line[0]]), end='')
    # else:
    #     raise UnknowCommand(line)

filename = 'data.txt'
if len(sys.argv) == 2:
    filename = sys.argv[1]
scanner(filename, processLine)
