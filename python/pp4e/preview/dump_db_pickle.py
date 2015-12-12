#!/usr/bin/python3

import pickle

f = open('people-pickle', 'rb')
db = pickle.load(f)

for key in db:
    print(key, '=>\n  ', db[key])
print(db['sue']['name'])
