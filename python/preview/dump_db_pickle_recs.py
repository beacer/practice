#!/usr/bin/python3

import pickle, glob

for filename in glob.glob('*.pkl'):
    f = open(filename, 'rb')
    rec = pickle.load(f)
    f.close
    print(filename, '=>\n ', rec)
