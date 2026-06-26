#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

#include "../include/launcher.h"
#include "../include/io.h"

// Set up redirection and exec the requested command.
// Supported operators are <, >, and >>.
void launch_process(char *argv[])
{
    char *input_file = NULL;
    char *output_file = NULL;
    int append = 0;

    for (int i = 0; argv[i] != NULL; i++)
    {
        if (strcmp(argv[i], "<") == 0)
        {
            if (argv[i + 1] == NULL)
            {
                shell_printf("Syntax error: missing input file\n");
                _exit(1);
            }

            input_file = argv[i + 1];
            argv[i] = NULL;
        }
        else if (strcmp(argv[i], ">") == 0)
        {
            if (argv[i + 1] == NULL)
            {
                shell_printf("Syntax error: missing output file\n");
                _exit(1);
            }

            output_file = argv[i + 1];
            argv[i] = NULL;
        }
        else if (strcmp(argv[i], ">>") == 0)
        {
            if (argv[i + 1] == NULL)
            {
                shell_printf("Syntax error: missing output file\n");
                _exit(1);
            }

            output_file = argv[i + 1];
            append = 1;
            argv[i] = NULL;
        }
    }

    if (output_file != NULL)
    {
        int fd;

        if (append)
            fd = open(output_file,
                      O_WRONLY | O_CREAT | O_APPEND,
                      0644);
        else
            fd = open(output_file,
                      O_WRONLY | O_CREAT | O_TRUNC,
                      0644);

        if (fd < 0)
        {
            perror("open");
            _exit(1);
        }

        dup2(fd, STDOUT_FILENO);
        close(fd);
    }

    if (input_file != NULL)
    {
        int fd = open(input_file, O_RDONLY);

        if (fd < 0)
        {
            perror("open");
            _exit(1);
        }

        dup2(fd, STDIN_FILENO);
        close(fd);
    }

    execvp(argv[0], argv);

    perror("Command failed");
    _exit(1);
}
