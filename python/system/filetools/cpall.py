#!/usr/bin/python3

import os, sys
maxfileload = 1000000
blksize = 1024 * 500

def copyfile(pathFrom, pathTo, maxfileload=maxfileload):
    """
    copy one file from pathFrom to pathTo
    use bin mode to prevent unicode decodeing and EOL transform
    """
    if os.path.getsize(pathFrom) < maxfileload:
        bytesFrom = open(pathFrom, 'rb').read()
        open(pathTo, 'wb').write(bytesFrom)
    else:
        fileFrom = open(pathFrom, 'rb')
        fileTo = open(pathTo, 'wb')
        while True:
            bytesFrom = fileFrom.read(blksize)
            if not bytesFrom: break
            fileTo.write(bytesFrom)

def copydir(dirFrom, dirTo, verbose = 0):
    """
    copy dirFrom to dirTo, return (file, dir) tuples.
    skip symbollink fifo .. in Unix system
    """
    fcount = dcount = 0
    for f in os.listdir(dirFrom):
        pathFrom = os.path.join(dirFrom, f)
        pathTo = os.path.join(dirTo, f)
        if not os.path.isdir(pathFrom):
            try:
                if verbose > 1: print('copying', pathFrom, 'to', pathTo)
                copyfile(pathFrom, pathTo)
                fcount += 1
            except:
                print('Error copying', pathFrom, 'to', pathTo, '--skipped')
                print(sys.exc_info()[0], sys.exc_info()[1])
        else:
            if verbose: print('copying dir', pathFrom, 'to', pathTo)
            try:
                os.mkdir(pathTo)
                below = copydir(pathFrom, pathTo)
                fcount += below[0]
                dcount += below[1]
                dcount += 1
            except:
                print('Error creating', pathTo, '--skipped')
                print(sys.exc_info()[0], sys.exc_info()[1])
    return (fcount, dcount)

def getargs():
    try:
        dirFrom, dirTo = sys.argv[1:]
    except:
        print('Usage error: cpall.py dirFrom dirTo')
    else:
        if not os.path.isdir(dirFrom):
            print('Error: dirFrom is not a directory')
        elif not os.path.exists(dirTo):
            os.mkdir(dirTo)
            print('Note: dirTo was created')
            return (dirFrom, dirTo)
        else:
            print('Warning: dirTo already exists')
            if hasattr(os.path, 'samefie'):
                same = os.path.samefile(dirFrom, dirTo)
            else:
                same = os.path.abspath(dirFrom) == os.path.abspath(dirTo)
            if same:
                print('Error: dirFrom same as dirTo')
            else:
                return (dirFrom, dirTo)

if __name__ == '__main__':
    import time
    dirstuple = getargs()
    if dirstuple:
        print('Copying...')
        start = time.clock()
        fcount, dcount = copydir(*dirstuple)
        print('Copied', fcount, 'files', dcount, 'directories', end='')
        print('in', time.clock() - start, 'seconds')
