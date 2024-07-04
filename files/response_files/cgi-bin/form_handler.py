#!/usr/bin/env python3

import cgi
import cgitb

# Enable debugging
cgitb.enable()

print("Content-type: text/html\n")
print("<html>")
print("<head>")
print("<title>Form Submission</title>")
print("</head>")
print("<body>")
print("<h2>CGI Script Running</h2>")

# Attempt to retrieve form data
try:
    form = cgi.FieldStorage()
    
    # Retrieve form data
    name = form.getvalue('name')
    email = form.getvalue('email')

    # Debug output
    print("<h3>Debug Information</h3>")
    print(f"<p>Name: {name}</p>")
    print(f"<p>Email: {email}</p>")
    
    if name and email:
        print("<h2>Form Submission Successful</h2>")
        print(f"<p>Name: {name}</p>")
        print(f"<p>Email: {email}</p>")
    else:
        print("<h2>Form Submission Failed</h2>")
        print("<p>Please provide both name and email.</p>")
    
except Exception as e:
    print("<h2>Error</h2>")
    print(f"<p>{str(e)}</p>")

print("</body>")
print("</html>")
