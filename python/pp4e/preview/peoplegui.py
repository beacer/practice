#!/usr/bin/python3

from tkinter import *
from tkinter.messagebox import showerror
import shelve

shelvename = 'class-shelve'
fieldnames = ('name', 'age', 'job', 'pay')

def makeWidgets():
    global entries
    window = Tk()
    window.title('People Shelve')
    form = Frame(window) # container @form's parent is @window
    form.pack()

    entries = {}
    for (ix, label) in enumerate(('key',) + fieldnames): # (index, label) pair
        lab = Label(form, text=label) # text label
        ent = Entry(form)
        lab.grid(row=ix, column=0)
        ent.grid(row=ix, column=1)
        entries[label] = ent # 'key', 'name', 'age', 'job', 'pay' entries

    Button(window, text='Fetch', command=fetchRecord).pack(side=LEFT)
    Button(window, text='Update', command=updateRecord).pack(side=LEFT)
    Button(window, text='Quit', command=window.quit).pack(side=RIGHT)
    return window;

def fetchRecord():
    key = entries['key'].get()
    try:
        rec = db[key]
    except:
        showerror(title='Error', message='No such key!')
    else:
        for field in fieldnames: # load fields from db
            entries[field].delete(0, END)
            entries[field].insert(0, repr(getattr(rec, field)))

def updateRecord():
    key = entries['key'].get()
    print('== key got', key)
    if key in db:
        rec = db[key]
    else: # new record
        from person import Person
        rec = Person(name='?', age='?')
        print('== new person', key)

    for field in fieldnames:
        setattr(rec, field, eval(entries[field].get()))
        print('== new rec', rec, field, eval(entries[field].get()))
    db[key] = rec

db = shelve.open(shelvename)
window = makeWidgets()
window.mainloop()
print('== getting close')
for k in db:
    print('== ', k, db[k])
db.close()
