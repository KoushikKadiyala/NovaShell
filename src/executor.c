#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#include "../include/executor.h"
#include "../include/launcher.h"
#include "../include/process.h"

// Decide whether the command includes a pipeline or is a single command.
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
        }
    }

    if (has_pipe)
    {
        execute_pipe_command(argv);
    }
    else
    {
        execute_simple_command(argv);
    }
}
