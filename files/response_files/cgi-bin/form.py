#!/usr/bin/env python3

import os
import cgi

body = '<html><body>\n<h1>Python CGI "Test form"</h1>\n'
method = os.environ.get("REQUEST_METHOD", "")

if method == "GET":
    body += '''<form method='POST' action='/cgi-bin/form.py'>
<label for='name'>Name:</label>
<input type='text' id='name' name='name'><br><br>
<label for='city'>City:</label>
<input type='city' id='city' name='city'><br><br>
<input type='submit' value='Submit'>
</form>
'''
elif method == "POST":
    form = cgi.FieldStorage()
    
    # Get form values and handle cases where they are lists
    name = form.getvalue("name", "Unknown person")
    if isinstance(name, list):
        name = name[0]  # Take the first value if it's a list
    
    city = form.getvalue("city", "Nowhere")
    if isinstance(city, list):
        city = city[0]  # Take the first value if it's a list
    
    body += f"<h2>Hello, {name} from {city}!</h2>"
else:
    body += f"<h2>Unknown method: {method}</h2>"

body += "</body></html>"

response = f"{body}"
print(response)








