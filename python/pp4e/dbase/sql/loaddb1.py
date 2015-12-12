#!/usr/bin/env python3

import sqlite3

conn = sqlite3.connect('dbase1')
curs = conn.cursor()

file = open('data.txt')

rows = [line.rstrip().split(', ') for line in file]
for rec in rows:
    curs.execute('insert into people values (?, ?, ?)', rec)

conn.commit()
conn.close()
