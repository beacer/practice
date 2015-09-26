#!/usr/bin/env python3

def summer(numCols, fileName):
    sums = [0] * numCols
    for line in open(fileName):
        cols = line.split()
        for i in range(numCols):
            sums[i] += int(cols[i])
    return sums

if __name__ == '__main__':
    import sys
    print(summer(eval(sys.argv[1]), sys.argv[2]))
