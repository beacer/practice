#!/usr/bin/python3

print("""Content-Type: text/html

<title>CGI 101</title>
<h1>A Third CGI Script</h1>
<hr>
<p>Hello, CGI World!</p>
<table border=1>
""")

for i in range(5):
    print("<tr>")
    for j in range(4):
        print("<td>%d.%d</td>" % (i, j))
    print("</tr>")

print("""
</table>
<hr>""")
