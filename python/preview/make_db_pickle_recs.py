#!/usr/bin/python3

from initdata import bob, sue, tom

import pickle
for (key, rec) in [('bob', bob), ('tom', tom), ('sue', sue)]:
    recfile = open(key + '.pkl', 'wb')
    pickle.dump(rec, recfile)
    recfile.close()
