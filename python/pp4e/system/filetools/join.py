#!/usr/bin/python3

import os, sys
readsize = 1024

def join(fromdir, tofile):
    output = open(tofile, 'wb')
    parts = os.listdir(fromdir)
    parts.sort()

    for filename in parts:
        filepath = os.path.join(fromdir, filename)
        fileobj = open(filepath, 'rb')
        while True:
            filebytes = fileobj.read(readsize)
            if not filebytes: break
            output.write(filebytes)
        fileobj.close()
    output.close()

if __name__ == '__main__':
    if len(sys.argv) == 2 and sys.argv[1] == '-help':
        print('Use: join.py [from-dir to-file]')
        sys.exit()

    if len(sys.argv) != 3:
        interative = True
        fromdir = input('Directory containing part files? ')
        tofile = input('Name of file to be recreated? ')
    else:
        interative = False
        fromdir, tofile = sys.argv[1:]

    absfrom, absto = map(os.path.abspath, [fromdir, tofile])
    print('Joining', absfrom, 'to make', absto)

    try:
        join(fromdir, tofile)
    except:
        print('Error joining files:')
        print(sys.exc_info()[0], sys.exc_info()[1])
    else:
        print('Join complete: see', absto)
    if interative: input('Press any key')

