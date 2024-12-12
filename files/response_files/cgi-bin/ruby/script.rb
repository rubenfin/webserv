#!/usr/bin/env ruby

require 'cgi'

cgi = CGI.new
# Generate the HTML content
puts <<-HTML
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>CGI Test</title>
</head>
<body>
    <h1>CGI Test Successful</h1>
    <p>If you're seeing this page, your Ruby CGI script is working!</p>
    <p>Current server time: <code>#{Time.now}</code></p>
</body>
</html>
HTML
