#!/usr/bin/python3

import pickle

f = open('people-pickle', 'rb')
db = pickle.load(f)
f.close()

db['sue']['pay'] *= 1.10
db['tom']['name'] = 'Tom Tom'

f = open('people-pickle', 'wb')
pickle.dump(db, f)
f.close()
