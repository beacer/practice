#!/usr/bin/python3

inputkey = 'language'
hellos = {
        'Python'    : r" print('hello, world')",
        'Python2'   : r" print 'hello, world' ",
        'Perl'      : r" print 'hello, world\n'; ",
        'Tcl'       : r" puts 'hello, world\n'; ",
        'Scheme'    : r" (display 'hello, world')(newline) ",
        'Samlltalk' : r" 'hello, world' print ",
        'Java'      : r" System.out.println('hello, world');",
        'C'         : r' print("hello, world\n"); ',
        'C++'       : r' cout << "hello, world" << endl; ',
        'Basic'     : r' 10 PRINT "hello, world\n" ',
        'Fortran'   : r" print *, 'hello, world\n' ",
        'Pascal'    : r" WriteLn('hello, world') ",
        }
