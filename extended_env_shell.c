#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

#define MAX_INPUT 1024
#define MAX_ARGS 64

void display_prompt(void);

int main(void)
{
    char buffer[MAX_INPUT];
    char *args[MAX_ARGS];
    ssize_t read_bytes;
    pid_t child_pid;
    int status;

    while (1)
    {
        display_prompt();

        read_bytes = read(STDIN_FILENO, buffer, MAX_INPUT);
        if (read_bytes == -1)
        {
            perror("read");
            exit(EXIT_FAILURE);
        }

        if (read_bytes == 0) // End of file (Ctrl+D)
        {
            printf("\n");
            break;
        }

        buffer[read_bytes - 1] = '\0'; // Remove the newline character

        // Tokenize the input into arguments
        char *token = strtok(buffer, " ");
        int i = 0;
        while (token != NULL && i < MAX_ARGS - 1)
        {
            args[i++] = token;
            token = strtok(NULL, " ");
        }
        args[i] = NULL; // Last element should be NULL for exec functions

        // Check if the command is "exit"
        if (strcmp(args[0], "exit") == 0)
        {
            printf("Exiting shell.\n");
            break;
        }
        // Check if the command is "env"
        else if (strcmp(args[0], "env") == 0)
        {
            char **env = environ;
            while (*env != NULL)
            {
                printf("%s\n", *env);
                env++;
            }
        }
        else
        {
            // Check if the command exists in the PATH
            char *path = getenv("PATH");
            if (path == NULL)
            {
                fprintf(stderr, "Error: PATH environment variable not set\n");
                continue;
            }

            char *path_copy = strdup(path);
            if (path_copy == NULL)
            {
                perror("strdup");
                exit(EXIT_FAILURE);
            }

            token = strtok(path_copy, ":");
            while (token != NULL)
            {
                char command_path[MAX_INPUT];
                snprintf(command_path, sizeof(command_path), "%s/%s", token, args[0]);

                if (access(command_path, X_OK) == 0)
                {
                    child_pid = fork();

                    if (child_pid == -1)
                    {
                        perror("fork");
                        exit(EXIT_FAILURE);
                    }

                    if (child_pid == 0) // Child process
                    {
                        if (execv(command_path, args) == -1)
                        {
                            perror(args[0]);
                            exit(EXIT_FAILURE);
                        }
                    }
                    else // Parent process
                    {
                        waitpid(child_pid, &status, 0);
                        break; // Break the loop after command execution
                    }
                }

                token = strtok(NULL, ":");
            }

            free(path_copy);
        }
    }

    return EXIT_SUCCESS;
}

void display_prompt(void)
{
    printf(":) ");
    fflush(stdout);
}
