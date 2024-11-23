#!/bin/bash

# Print the HTML content
cat <<EOF
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>CGI Test</title>
</head>
<body>
    <h1>CGI Test Successful</h1>
    <p>If you're seeing this page, your CGI script is working!</p>
    <p>Current server time: <code>$(date)</code></p>
</body>
</html>
EOF
