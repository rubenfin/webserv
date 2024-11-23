#!/usr/bin/env php
<?php
// Generate the HTML content
echo <<<HTML
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
    <p>Current server time: <code>
HTML;

// Print the current server time
echo date('Y-m-d H:i:s');

echo <<<HTML
    </code></p>
</body>
</html>
HTML;
?>
