#!/usr/bin/python3

import sys

dbfilename = 'people-file'
ENDDB = 'dbend.'
ENDREC = 'recend.'
RECSEP = '=>'

def storeDbase(db, filename=dbfilename):
    """ save the formated db to file """
    f = open(filename, 'w')
    for key in db:
        print(key, file=f)
        for (name, value) in db[key].items():
            print(name + RECSEP + repr(value), file=f)
        print(ENDREC, file=f)
    print(ENDDB, file=f)
    f.close()

def loadDbase(filename=dbfilename):
    """ parse the record file build the db """
    f = open(filename, "r")
    sys.stdin = f
    db = {}
    key = input()
    while key != ENDDB:
        rec = {}
        field = input()
        while field != ENDREC:
            name, value = field.split(RECSEP)
            rec[name] = eval(value) # not safe
            field = input()
        db[key] = rec
        key = input()
    return db
        
if __name__ == '__main__':
    from initdata import db
    storeDbase(db)
