#!/bin/bash

while true; do
    # Display prompt
    echo -n "#cisfun$ "

    # Read user input
    read command

    # Check for end of file (Ctrl+D)
    if [ -z "$command" ]; then
        echo "Exiting simple_shell. Goodbye!"
        exit 0
    fi

    # Execute the command using execve
    if [ -x "$command" ]; then
        exec "$command"
    else
        echo "./shell: No such file or directory"
    fi
	done
