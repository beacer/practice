#!/usr/bin/python3

# records
bob = dict(name='Bob Smith', age=42, pay=3000, job='dev')
sue = dict(name='Sue Jones', age=45, pay=4000, job='hdw')
tom = dict(name='Tom', age=50, pay=0, job=None)

# database
db = {}
db['bob'] = bob
db['sue'] = sue
db['tom'] = tom

if __name__ == '__main__': # run as a script
    for key in db:
        print(key, '=>\n ', db[key])
