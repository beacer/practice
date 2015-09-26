#!/usr/bin/python3

import cgi, sys

debugme = False
inputkey = "language"

hellos = { 
        "Python":     r"print('hello, world)'                 ",
        "Python2":    r"print 'hello, world'                  ",
        "Perl":       r"print 'hello, world\n';               ",
        "Tcl":        r"puts  'hello, world'                  ",
        "Scheme":     r"(display  'hello, world')(new line)   ",
        "Smalltalk":  r"'hello, world' print.                 ",
        "Java":       r"System.out.println('hello, world');   ",
        "C":          r'printf("hello, world\n");             ',
        "C++":        r'cout << "hello, world\n" << endl;     ',
        "Basic":      r'10 PRINT "Hello World"                ',
        "Fortran":    r"print *, 'Hello World'                ",
        "Pascal":     r"WriteLn('Hello World')                ",
        }

class dummy:
    def __init__(self, str):
        self.value = str

if debugme:
    form = {inputkey: dummy(sys.argv[1])}
else:
    form = cgi.FieldStorage()

print('Content-Type: text/html\r\n')
print('<title>Languages</title>')
print('<h1>Syntax</h1>')

def show_hello(form):
    choice = form['language'].value
    print('<h3>%s</h3><p><pre>' % choice)
    try:
        print(cgi.escape(hellos[choice])) # output as HTML
    except KeyError:
        print('Sorry -- I don\' know that language')
    print('</pre></p><br>')

if not inputkey in form or form[inputkey].value == "All":
    for lang in hellos.keys():
        mock = {inputkey: dummy(lang)}
        show_hello(mock)
else:
    show_hello(form)
print("<hr/>");
