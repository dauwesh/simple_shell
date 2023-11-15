#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_INPUT 1024
void display_prompt(void);
int main(void)
{
char buffer[MAX_INPUT];
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
if (read_bytes == 0)
}
printf("\n");
break;
}
buffer[read_bytes - 1] = '\0';
child_pid = fork();
if (child_pid == -1)
{
perror("fork");
exit(EXIT_FAILURE);
}
if (child_pid == 0)
{
if (execlp(buffer, buffer, (char *)NULL) == -1)
{
perror(buffer);
exit(EXIT_FAILURE);
}
}
else
{
waitpid(child_pid, &status, 0);
}
}
return (EXIT_SUCCESS);
}
void display_prompt(void)
{
printf("#cisfun$ ");
fflush(stdout);
}
