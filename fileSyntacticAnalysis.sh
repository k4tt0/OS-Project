#!/bin/bash

# Check if a filename is provided as an argument
if [ $# -ne 1 ]; then
    echo "Usage: $0 <filename>"
    exit 1
fi

filename="$1"

# Check if the file exists
if [ ! -f "$filename" ]; then
    echo "Error: File '$filename' does not exist."
    exit 1
fi

# Count lines, words, and characters
lines=$(wc -l < "$filename")
words=$(wc -w < "$filename")
chars=$(wc -m < "$filename")

# Search for specific words
search_words=("corrupted" "dangerous" "risk" "attack" "malware" "malicious")
found=false

for word in "${search_words[@]}"; do
    if grep -q "$word" "$filename"; then
        found=true
        break
    fi
done

# Output results
echo "Lines: $lines"
echo "Words: $words"
echo "Characters: $chars"

if [ "$found" = true ]; then
    echo "Positive response: At least one of the specified words was found."
else
    echo "Negative response: None of the specified words were found."
fi
