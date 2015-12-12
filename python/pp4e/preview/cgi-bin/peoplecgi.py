#!/usr/bin/python3

import cgi, shelve, sys, os
shelvename = 'class-shelve'
fieldnames = ('name', 'age', 'pay', 'job')

form = cgi.FieldStorage()   # parse form to form{}, key is <th/> value is <td/>, plush form['action']
print('Content-Type: text/html\n')
sys.path.insert(0, os.getcwd())  # add cwd to search path, L.insert(): insert obj "os.getcwd" befor index "0"

# html template
# %(key)s can be used to format string
# 'I'm %(age)s years old' %s {'age': 32, 'name': 'Tom'}
replyhtml = """
<html>
<title>People Input Form</title>
<body>
<form method=POST action="peoplecgi.py">
  <table>
    <tr><th>key<td><input type=text name=key value="%(key)s">
    $ROWS$
  </table>
  <p>
    <input type=submit value="Fetch", name=action>
    <input type=submit value="Update", name=action>
  </p>
</form>
</body>
</html>
"""

# insert html for $ROWS$
rowhtml = '<tr><th>%s<td><input type=text name=%s value="%%(%s)s">\n'
rowshtml = ''
for field in fieldnames:
    rowshtml += (rowhtml % ((field,) * 3))
replyhtml = replyhtml.replace('$ROWS$', rowshtml)

def htmlize(adict):
    new = adict.copy()
    for f in fieldnames:
        value = new[field]
        new[field] = cgi.escape(repr(value))
    return new

def fetchRecord(db, form):
    try:
        key = form['key'].value
        rec = db[key] # it's class Person's instance
        # rec.__dict__ returns the {'attr':value} of class rec
        # which can be used to build a new dict{} fields
        #   >>> tom  = Person('Tom', age=32)
        #   >>> tom.__dict__
        #   {'name': 'Tom', 'age': 32, 'pay': 0, 'job': None}
        fields = rec.__dict__
        fields['key'] = key
    except:
        fields = dict.fromkeys(fieldnames, '?')
        fields['key'] = 'Missing or invalid key!'
    return fields

def updateRecord(db, form):
    if not 'key' in form:
        fields = dict.fromkeys(fieldnames, '?')
        fields['key'] = 'Missing key input!'
    else:
        key = form['key'].value
        if key in db:
            rec = db[key]
        else:
            from person import Person
            rec = Person(name='?', age='?')

        for f in fieldnames:
            setattr(rec, f, eval(form[field].value))
        db[key] = rec
        fields = rec.__dict__
        fields['key'] = key
    return fields

db = shelve.open(shelvename)
action = form['action'].value if 'action' in form else None
if action == 'Fetch':
    fields = fetchRecord(db, form)
elif action == 'Update':
    fields = updateRecord(db, form)
else:
    fields = dict.fromkeys(fieldnames, '?') # dict.fromkeys() return 'name'='?', 'age'='?', ...
    fields['key'] = 'Missing or invalid action!'

db.close()
print(replyhtml % htmlize(fields))
