#!/usr/bin/python3

import sys, os, pprint

trace = False
# if len(sys.argv) < 1:
#     print('usage', sys.argv[0], '<dir>')
#     sys.exit(1)
# dirname = sys.argv[1]
try:
    dirname = sys.argv[1]
except IndexError:
    print('usage', sys.argv[0], '<dir>')
    sys.exit(1)

allsizes = []
for (thidDir, subsHere, filesHere) in os.walk(dirname):
    if trace: print(thidDir)
    for filename in filesHere:
        if trace: print('...', filename)
        fullname = os.path.join(thidDir, filename)
        fullsize = os.path.getsize(fullname)
        allsizes.append((fullsize, fullname))

allsizes.sort()
pprint.pprint(allsizes[:2])
pprint.pprint(allsizes[-2:])
