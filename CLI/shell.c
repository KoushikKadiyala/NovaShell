#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

#include <readline/readline.h>
#include <readline/history.h>

#include "../include/colours.h"
#include "../include/core.h"
#include "../include/io.h"

/**
 * start_shell - Main shell loop and command dispatcher
 *
 * Implements the core shell event loop:
 * 1. Ignores SIGINT (Ctrl+C) so the shell persists when child processes are interrupted
 * 2. Displays a colored prompt with the current working directory
 * 3. Reads user input using GNU Readline (provides history and editing)
 * 4. Expands environment variables and parses the command
 * 5. Routes the command to builtins or external executors
 * 6. Continues until user exits or EOF is reached
 *
 * Return: void (loop continues until exit command is given)
 */
void start_shell(void)
{
    /* Ignore SIGINT in the shell process so Ctrl+C does not exit the shell itself.
       Instead, the signal is delivered to child processes. */
    signal(SIGINT, SIG_IGN);
    rl_bind_key('\t', rl_complete);

    while (1)
    {
        char cwd[1024];
        char prompt[1200];

        /* Build a colored prompt showing the current working directory */
        if (getcwd(cwd, sizeof(cwd)) != NULL)
        {
            snprintf(
                prompt,
                sizeof(prompt),
                "\001" BLUE BOLD "\002%s \001" RESET "\002\001" RED BOLD "\002❯ \001" RESET "\002",
                cwd
            );
        }
        else
        {
            strcpy(prompt, "$ ");
        }

        /* Read a line from the user with GNU Readline support */
        char *input = readline(prompt);

        /* Check for EOF (Ctrl+D) or input failure */
        if (input == NULL)
        {
            shell_print("\nGoodbye!\n");
            break;
        }

        /* Add non-empty commands to readline history */
        if (strlen(input) > 0)
        {
            add_history(input);
        }

        /* Execute the command; returns 1 if exit command was given */
        if (shell_execute(input))
        {
            free(input);
            break;
        }

        free(input);
    }
}