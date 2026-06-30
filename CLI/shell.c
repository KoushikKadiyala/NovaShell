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

// Main shell loop: read input, parse, expand variables, handle builtins, execute commands.
void start_shell()
{
    // Ignore SIGINT in the shell process so Ctrl+C does not exit the shell itself.
    signal(SIGINT, SIG_IGN);

    while (1)
    {
        char cwd[1024];
        char prompt[1200];

        if (getcwd(cwd, sizeof(cwd)) != NULL)
        {
            snprintf(
                prompt,
                sizeof(prompt),
                "\001" BLUE BOLD "\002%s \001" RESET "\002\001" RED BOLD "\002> \001" RESET "\002",
                cwd
            );
        }
        else
        {
            strcpy(prompt, "$ ");
        }

        char *input = readline(prompt);

        if (input == NULL)
        {
            shell_print("\nGoodbye!\n");
            break;
        }

        if (strlen(input) > 0)
        {
            add_history(input);
        }

       if (shell_execute(input))
        {
            free(input);
            break;
        }
        free(input);
    }
}