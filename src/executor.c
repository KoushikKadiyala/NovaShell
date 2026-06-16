#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#include "../include/executor.h"
#include "../include/launcher.h"
#include "../include/process.h"

// Decide whether the command includes a pipeline or is a single command.
static int is_operator(const char *token)
{
    return token == NULL || strcmp(token, "|") == 0 || strcmp(token, "<") == 0 || strcmp(token, ">") == 0 || strcmp(token, ">>") == 0;
}

static int validate_command_segment(char *argv[])
{
    if (argv[0] == NULL)
    {
        printf("Syntax error: empty command\n");
        return 0;
    }

    if (is_operator(argv[0]))
    {
        printf("Syntax error: invalid command start '%s'\n", argv[0]);
        return 0;
    }

    return 1;
}

void execute_command(char *argv[])
{
    int has_pipe = 0;
    int i;

    for (i = 0; argv[i] != NULL; i++)
    {
        if (strcmp(argv[i], "|") == 0)
        {
            has_pipe = 1;
            
            if (i == 0)
            {
                printf("Syntax error before pipe\n");
                return;
            }

            if (argv[i + 1] == NULL)
            {
                printf("Syntax error after pipe\n");
                return;
            }

            if (is_operator(argv[i + 1]))
            {
                printf("Syntax error: invalid command start '%s'\n", argv[i + 1]);
                return;
            }
        }
    }

    if (has_pipe)
    {
        execute_pipe_command(argv);
    }
    else
    {
        if (!validate_command_segment(argv))
            return;

        execute_simple_command(argv);
    }
}
