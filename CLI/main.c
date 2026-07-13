#include "../include/shell.h"
#include "../include/io.h"

#include <stdio.h>

/**
 * console_output - Output handler for shell messages
 * @text: The text to output to stdout
 *
 * Writes text to standard output and flushes the buffer to ensure
 * immediate display of shell prompts and output.
 */
static void console_output(const char *text)
{
    fputs(text, stdout);
    fflush(stdout);
}

/**
 * main - Entry point for the NovaShell CLI executable
 *
 * Initializes the shell output handler and starts the main shell loop.
 * The shell handles user input, command parsing, and execution.
 *
 * Return: Always returns 0 (success)
 */
int main(void)
{
    shell_set_output(console_output);
    start_shell();
    return 0;
}