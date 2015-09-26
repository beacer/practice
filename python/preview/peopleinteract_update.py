#!/usr/bin/python3

import shelve

from person import Person
fieldnames = ('name', 'age', 'pay', 'job')

db = shelve.open('class-shelve')
while True:
    key = input('\nKey? => ')
    if not key: break; # empty to exit
    if key in db:
        rec = db[key]
    else:
        rec = Person(name = '?', age = '?')

    for field in fieldnames:
        currval = getattr(rec, field)
        newtext = input('\t[%s]=%s\n\t\tnew?=>' % (field, currval))
        if newtext:
            setattr(rec, field, eval(newtext))

    db[key] = rec

db.close()
