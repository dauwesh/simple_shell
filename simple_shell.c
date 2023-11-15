#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_COMMAND_LENGTH 100

void display_prompt() {
    printf("#cisfun$ ");
}

int main() {
    char command[MAX_COMMAND_LENGTH];

    while (1) {
        // Display prompt and wait for user input
        display_prompt();
        if (fgets(command, sizeof(command), stdin) == NULL) {
            // Handle end of file (Ctrl+D)
            printf("\nExiting simple_shell. Goodbye!\n");
            exit(0);
        }

        // Remove the newline character from the end of the command
        size_t command_length = strlen(command);
        if (command_length > 0 && command[command_length - 1] == '\n') {
            command[command_length - 1] = '\0';
        }

        // Execute the command using fork and execve
        pid_t pid = fork();

        if (pid == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        }

        if (pid == 0) {
            // Child process
            if (execlp(command, command, (char *)NULL) == -1) {
                // Print an error message if the executable is not found
                fprintf(stderr, "./shell: No such file or directory\n");
                exit(EXIT_FAILURE);
            }
        } else {
            // Parent process
            int status;
            waitpid(pid, &status, 0);

            if (WIFEXITED(status)) {
                // Display prompt again after command execution
                continue;
            } else {
                fprintf(stderr, "Error: Child process terminated abnormally\n");
                exit(EXIT_FAILURE);
            }
        }
    }

    return 0;
}
