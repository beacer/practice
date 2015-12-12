#!/usr/bin/python3

import os, glob, sys

#dirname = sys.argv[1] if len(sys.argv) > 1 else print('usage:', sys.argv[0], '<dir>')
if len(sys.argv) > 1:
    dirname = sys.argv[1]
else:
    print('usage:', sys.argv[0], '<dir>')
    sys.exit(1)

allsizes = []
allpy = glob.glob(dirname + os.sep + '*.py')
for filename in allpy:
    filesize = os.path.getsize(filename)
    allsizes.append((filesize, filename))

allsizes.sort()  # sort in-place, if elem is 2-tuple, use first tuple for sort
print(allsizes[:2])
print(allsizes[-2:])

