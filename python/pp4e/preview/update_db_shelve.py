#!/usr/bin/python3

from initdata import tom
import shelve

db = shelve.open('people-shelve')
sue = db['sue']
sue['pay'] *= 1.50
db['sue'] = sue
#db['sue']['pay'] *= 1.50
db['tom'] = tom
db.close
