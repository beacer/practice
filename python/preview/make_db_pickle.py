#!/usr/bin/python3

from initdata import db
import pickle

f = open('people-pickle', 'wb')
pickle.dump(db, f)
f.close()
