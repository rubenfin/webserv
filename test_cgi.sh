#!/bin/bash

export REQUEST_METHOD=POST
export CONTENT_TYPE=application/x-www-form-urlencoded
export CONTENT_LENGTH=28

# Simulate POST data
echo "name=test&email=test%40testt" | files/response_files/cgi-bin/form_handler.py
