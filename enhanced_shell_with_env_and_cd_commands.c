#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

#define MAX_INPUT 1024
#define MAX_ARGS 64

void display_prompt(void);
ssize_t custom_getline(char **lineptr, size_t *n);
int tokenize_input(char *input, char **args);
int execute_command(char **args);
void update_pwd(char *new_path);

int main(void) {
    char *buffer = NULL;
    size_t buffer_size = 0;
    ssize_t read_bytes;
    int status = 0; // Default status for exit command

    while (1) {
        display_prompt();

        read_bytes = custom_getline(&buffer, &buffer_size);
        if (read_bytes == -1) {
            perror("read");
            exit(EXIT_FAILURE);
        }

        if (read_bytes == 0) { // End of file (Ctrl+D)
            printf("\n");
            break;
        }

        buffer[read_bytes - 1] = '\0'; // Remove the newline character

        char *args[MAX_ARGS];
        int argc = tokenize_input(buffer, args);

        // Check if the command is "exit"
        if (strcmp(args[0], "exit") == 0) {
            if (argc > 1) {
                status = atoi(args[1]); // Convert the argument to an integer
            }
            printf("Exiting shell with status %d.\n", status);
            break;
        }

        // Check if the command is "env"
        if (strcmp(args[0], "env") == 0) {
            char **env = environ;
            while (*env != NULL) {
                printf("%s\n", *env);
                env++;
            }
        }
        // Check if the command is "setenv"
        else if (strcmp(args[0], "setenv") == 0) {
            if (argc != 3) {
                fprintf(stderr, "Usage: setenv VARIABLE VALUE\n");
            } else {
                if (setenv(args[1], args[2], 1) != 0) {
                    perror("setenv");
                }
            }
        }
        // Check if the command is "unsetenv"
        else if (strcmp(args[0], "unsetenv") == 0) {
            if (argc != 2) {
                fprintf(stderr, "Usage: unsetenv VARIABLE\n");
            } else {
                if (unsetenv(args[1]) != 0) {
                    perror("unsetenv");
                }
            }
        }
        // Check if the command is "cd"
        else if (strcmp(args[0], "cd") == 0) {
            char *new_path = (argc == 1 || strcmp(args[1], "~") == 0) ? getenv("HOME") : args[1];

            if (strcmp(args[1], "-") == 0) {
                new_path = getenv("OLDPWD");
                if (new_path == NULL) {
                    fprintf(stderr, "OLDPWD not set\n");
                    continue;
                }
            }

            char current_path[MAX_INPUT];
            if (getcwd(current_path, sizeof(current_path)) == NULL) {
                perror("getcwd");
                exit(EXIT_FAILURE);
            }

            if (chdir(new_path) != 0) {
                perror("chdir");
            } else {
                update_pwd(current_path);
            }
        } else {
            execute_command(args);
        }
    }

    free(buffer);

    return status; // Return the exit status
}

void display_prompt(void) {
    printf(":) ");
    fflush(stdout);
}

ssize_t custom_getline(char **lineptr, size_t *n) {
    static char buffer[MAX_INPUT];
    static char *buf_pos = buffer; // Position in the buffer
    ssize_t total_bytes = 0;
    ssize_t bytes_read;

    if (lineptr == NULL || n == NULL) {
        fprintf(stderr, "Invalid arguments\n");
        return -1;
    }

    // Initialize the buffer if it's the first call
    if (buf_pos == buffer) {
        bytes_read = read(STDIN_FILENO, buffer, MAX_INPUT);
        if (bytes_read == -1) {
            perror("read");
            return -1;
        }
        if (bytes_read == 0) {
            // End of file
            return 0;
        }
    }

    // Read characters from the buffer until a newline or end of buffer
    while (*buf_pos != '\n' && total_bytes < MAX_INPUT) {
        (*lineptr)[total_bytes++] = *buf_pos++;

        // If we reach the end of the buffer, read more characters
        if (buf_pos == buffer + MAX_INPUT) {
            bytes_read = read(STDIN_FILENO, buffer, MAX_INPUT);
            if (bytes_read == -1) {
                perror("read");
                return -1;
            }
            if (bytes_read == 0) {
                // End of file
                break;
            }
            buf_pos = buffer; // Reset buffer position
        }
    }

    (*lineptr)[total_bytes] = '\0'; // Null-terminate the string

    // Update the size of the buffer
    *n = MAX_INPUT;

    return total_bytes;
}

int tokenize_input(char *input, char **args) {
    int argc = 0;
    char *token = NULL;
    char *rest = input;

    while ((token = strtok_r(rest, " ", &rest)) != NULL && argc < MAX_ARGS - 1) {
        args[argc++] = token;
    }

    args[argc] = NULL; // Last element should be NULL for exec functions

    return argc;
}

int execute_command(char **args) {
    pid_t child_pid;
    int status;

    // Check if the command exists in the PATH
    char *path = getenv("PATH");
    if (path == NULL) {
        fprintf(stderr, "Error: PATH environment variable not set\n");
        return -1;
    }

    char *path_copy = strdup(path);
    if (path_copy == NULL) {
        perror("strdup");
        exit(EXIT_FAILURE);
    }

    char *token = NULL;
    char *rest = path_copy;
    while ((token = strtok_r(rest, ":", &rest)) != NULL) {
        char command_path[MAX_INPUT];
        snprintf(command_path, sizeof(command_path), "%s/%s", token, args[0]);

        if (access(command_path, X_OK) == 0) {
            child_pid = fork();

            if (child_pid == -1) {
                perror("fork");
                exit(EXIT_FAILURE);
            }

            if (child_pid == 0) { // Child process
                if (execv(command_path, args) == -1) {
                    perror(args[0]);
                    exit(EXIT_FAILURE);
                }
            } else { // Parent process
                waitpid(child_pid, &status, 0);
                break; // Break the loop after command execution
            }
        }
    }

    free(path_copy);

    return status;
}

void update_pwd(char *new_path) {
    char current_path[MAX_INPUT];

    if (getcwd(current_path, sizeof(current_path)) == NULL) {
        perror("getcwd");
        exit(EXIT_FAILURE);
    }

    if (setenv("OLDPWD", current_path, 1) != 0) {
        perror("setenv");
        exit(EXIT_FAILURE);
    }

    if (setenv("PWD", new_path, 1) != 0) {
        perror("setenv");
        exit(EXIT_FAILURE);
    }
}
