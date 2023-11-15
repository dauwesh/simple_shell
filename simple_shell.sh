#!/bin/bash

while true; do
    # Display prompt
    echo -n "simple_shell> "

    # Read user input
    read command

    # Check for end of file (Ctrl+D)
    if [ -z "$command" ]; then
        echo "Exiting simple_shell. Goodbye!"
        exit 0
    fi

    # Execute the command using execve
    if command -v "$command" &> /dev/null; then
        exec "$command"
    else
        echo "Error: Command not found: $command"
    fi
	done
