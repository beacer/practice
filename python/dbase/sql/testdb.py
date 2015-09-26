#!/usr/bin/env python3

import sqlite3

conn = sqlite3.connect('dbase1')
curs = conn.cursor()

try:
    curs.execute('drop table people')
except:
    pass

curs.execute('create table people (name char(30), job char(30), pay int(4))')
curs.execute('insert into people values (?, ?, ?)', ('Bob', 'dev', 50000))
curs.execute('insert into people values (?, ?, ?)', ('Sue', 'dev', 60000))

curs.execute('select * from people')
for row in curs.fetchall():
    print(row)


curs.execute('select * from people')
colnames = [desc[0] for desc in curs.description]
while True:
    print('-' * 30)
    row = curs.fetchone()
    if not row: break
    for (name, value) in zip(colnames, row):
        print('%s => %s' % (name, value))

conn.commit()
