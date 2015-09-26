#!/usr/bin/python3

import cgi

form = cgi.FieldStorage()
print('Content-Type: text/html\r\n')
html = """<title>tutor3.py</title>
<h1>Greetings<h1>
<hr>
<p>%s</p>
<hr>"""

if not 'user' in form:
    print(html % 'Who are you?')
else:
    print(html % ("hello, %s." % form['user'].value))
