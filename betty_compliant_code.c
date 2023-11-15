#include "header_file.h"
/**
 * main - Entry point of the program
 *
 * Return: Always 0 (Success)
 */
int main(void)
{
char *message = "Hello, Betty!";
print_message(message);
return (0);
}
/**
 * print_message - Prints a message to the standard output
 *
 * @msg: The message to be printed
 */
void print_message(char *msg)
{
if (msg != NULL)
{
while (*msg)
{
putchar(*msg);
msg++;
}
putchar('\n');
}
}
