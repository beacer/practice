#!/usr/bin/python3

import cgi

form = cgi.FieldStorage()
print('Content-Type: text/html\n')
print('<title>Reply Page</title>\n')

if not 'user' in form:
    print('<h1>Whw are you?</h1>')
else:
    print('<h1>Hello <i>%s</i>!</h1>' % cgi.escape(form['user'].value))
