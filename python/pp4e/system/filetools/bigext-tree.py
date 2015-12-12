#!/usr/bin/python3

import os, pprint
from sys import argv, exc_info

trace = 1
dirname, extname = os.curdir, '.py'

if len(argv) > 1: dirname = argv[1]
if len(argv) > 2: extname = argv[2]
if len(argv) > 3: trace = int(argv[3])

def tryprint(arg):
    try:
        print(arg)
    except UnicodeEncodeError:
        print(arg.encode())

visited = set()
allsizes = []

for (thisDir, subsHere, filesHere) in os.walk(dirname):
    if trace: tryprint(thisDir)
    thisDir = os.path.normpath(thisDir)
    fixname = os.path.normcase(thisDir)
    if fixname in visited:
        continue
    else:
        visited.add(fixname)
    for filename in filesHere:
        if filename.endswith(extname):
            if trace > 1: tryprint('+++' + extname)
            fullname = os.path.join(thisDir, filename)
            try:
                bytesize = os.path.getsize(fullname)
                linesize = sum(+1 for line in open(fullname, 'rb'))
            except Exception:
                print('error', exc_info[0])
            else:
                allsizes.append((bytesize, linesize, fullname))

for (title, key) in [('bytes', 0), ('lines', 1)]:
    print('\nBy %s...' % title)
    allsizes.sort(key=lambda x: x[key])
    pprint.pprint(allsizes[:3])
    pprint.pprint(allsizes[-3:])


