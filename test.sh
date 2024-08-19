#!/bin/bash

# Default values
URL="http://localhost:8000"
METHOD="GET"
DATA=""

# Parse command line arguments
while getopts u:m:d: flag
do
    case "${flag}" in
        u) URL=${OPTARG};;
        m) METHOD=${OPTARG};;
        d) DATA=${OPTARG};;
        *) echo "Usage: $0 [-u URL] [-m METHOD] [-d DATA]"; exit 1;;
    esac
done

# Perform the request
if [ "$METHOD" == "GET" ]; then
    curl -X $METHOD "$URL"
else
    curl -X $METHOD -d "$DATA" "$URL"
fi

# Check if the request was successful
if [ $? -eq 0 ]; then
    echo "Request to $URL completed successfully."
else
    echo "Failed to make request to $URL."
fi
