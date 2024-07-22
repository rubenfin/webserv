#!/usr/bin/env python3
import cgi
import html
import sys
import os
import urllib.parse

try:
    # Get data from QUERY_STRING for both GET and POST requests
    query_string = os.environ.get('QUERY_STRING', '')
    form_data = urllib.parse.parse_qs(make)

    # Get data from parsed form
    name = form_data.get('name', [''])[0]
    email = form_data.get('email', [''])[0]

    # Print HTML content
    print("<h2>Form Submission Result:</h2>")
    print(f"<p>Name: {html.escape(name)}</p>")
    print(f"<p>Email: {html.escape(email)}</p>")

except Exception as e:
    print("<h2>An error occurred:</h2>")
    print(f"<p>{html.escape(str(e))}</p>")
    print("<p>Please check the server logs for more details.</p>")
    print(f"<p>Python version: {sys.version}</p>")
    print("<h3>Environment Variables:</h3>")
    for key, value in os.environ.items():
        print(f"<p>{html.escape(key)}: {html.escape(value)}</p>")
    print("<h3>Query String:</h3>")
    print(f"<p>{html.escape(query_string)}</p>")