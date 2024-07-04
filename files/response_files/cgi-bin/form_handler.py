#!/usr/bin/env python3

import os
import sys
import cgi
import cgitb
import urllib.parse

cgitb.enable()  # for debugging

print("Content-Type: text/html\n")  # Ensure that the Content-Type header is printed

# Check if the request method is POST
if os.environ.get('REQUEST_METHOD', '') == 'POST':
    content_length = int(os.environ.get('CONTENT_LENGTH', 0))
    post_data = sys.stdin.read(content_length)
    form_data = urllib.parse.parse_qs(post_data)
    
    # Get data from fields
    name = form_data.get('name', [''])[0]
    email = form_data.get('email', [''])[0]
else:
    name = ''
    email = ''

# Generate HTML response
print(f"""
<html>
<head>
    <title>Form Submission</title>
</head>
<body>
    <h2>Form Submission Received</h2>
    <p>Name: {name}</p>
    <p>Email: {email}</p>
</body>
</html>
""")
