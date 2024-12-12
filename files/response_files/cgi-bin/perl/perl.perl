#!/usr/bin/env perl

use strict;
use warnings;

# Print the HTML content
print <<'HTML';
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>CGI Test</title>
</head>
<body>
    <h1>CGI Test Successful</h1>
    <p>If you're seeing this page, your Perl CGI script is working!</p>
    <p>Current server time: <code>
HTML

# Print the current time
print scalar localtime;

# Close the HTML
print <<'HTML';
    </code></p>
</body>
</html>
HTML
