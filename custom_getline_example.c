#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

/**
 * _getline - Custom getline function
 * @lineptr: Pointer to the buffer where the line will be stored
 * @n: Pointer to the size of the buffer
 *
 * Return: Number of characters read, or -1 on failure
 */
ssize_t _getline(char **lineptr, size_t *n) {
    static char buffer[BUFFER_SIZE];
    static char *buf_pos = buffer; // Position in the buffer
    ssize_t total_bytes = 0;
    ssize_t bytes_read;

    if (lineptr == NULL || n == NULL) {
        fprintf(stderr, "Invalid arguments\n");
        return -1;
    }

    // Initialize the buffer if it's the first call
    if (buf_pos == buffer) {
        bytes_read = read(STDIN_FILENO, buffer, BUFFER_SIZE);
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
    while (*buf_pos != '\n' && total_bytes < BUFFER_SIZE) {
        (*lineptr)[total_bytes++] = *buf_pos++;

        // If we reach the end of the buffer, read more characters
        if (buf_pos == buffer + BUFFER_SIZE) {
            bytes_read = read(STDIN_FILENO, buffer, BUFFER_SIZE);
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
    *n = BUFFER_SIZE;

    return total_bytes;
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    ssize_t read_bytes;

    while ((read_bytes = _getline(&line, &len)) != -1) {
        if (read_bytes == 0) {
            // End of file
            break;
        }

        printf("Line: %s", line);
    }

    free(line);

    return 0;
}
