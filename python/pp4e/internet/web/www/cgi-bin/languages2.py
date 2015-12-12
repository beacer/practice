#!/usr/bin/python3

REPLY = """Content-Type: text/html

<html>
<title>Languages</title>
<body>
<h1>Hello World Selector</h1>
<p>This demo shows how to display a "hell world" message in various
programming languages' syntax.

<ul>
    <li>To see this page's HTML, use the 'View Source' command in your browser.</li>
    <li>To view Python CGI script on the server, 
        <a href="http://localhost:8080/cgi-bin/languages-src.py">click here</a> or 
        <a href="http://localhost:8080/cgi-bin/getfile.py?filename=cgi-bin/languages.py">here</a>
    </li>
    <li>To see alternative version that generates this page dynamically,
        <a href="http://localhost:8080/cgi-bin/languages2.py">click here</a>.
    </li>
</ul>
</p>

<hr/>
<form method=POST action="http://localhost:8080/cgi-bin/languages2reply.py">
    <p><b>Select a programming language:</b></p>
    <p><select name=%s>
        <option>All</option>
        %s
        <option>Other</option>
    </select>
    </p><input type=submit />
</form>
</body>
</html>
"""

from languages2common import hellos, inputkey

options = []
for lang in hellos:
    options.append('<option>' + lang + '</option>')
options = '\n\t'.join(options)
print(REPLY % (inputkey, options))


