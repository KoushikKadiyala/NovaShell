#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#include "../include/process.h"
#include "../include/launcher.h"

void execute_simple_command(char *argv[])
{
    pid_t pid = fork();

    if (pid < 0)
    {
        perror("fork");
        return;
    }

    if (pid == 0)
    {
        launch_process(argv);
    }

    waitpid(pid, NULL, 0);
}

void execute_pipe_command(char *argv[])
{
    char **commands[100];
    int commands_count = 1;
    commands[0] = argv;

    for (int i = 0; argv[i] != NULL; i++)
    {
        if (strcmp(argv[i], "|") == 0)
        {
            argv[i] = NULL;
            commands[commands_count++] = &argv[i + 1];
        }
    }
    commands[commands_count] = NULL;

    int prev_read = -1;
    pid_t pids[100];
    for (int i = 0; i < commands_count; i++)
    {
        int pipefd[2];
        if (i < commands_count - 1)
        {
            if (pipe(pipefd) < 0)
            {
                perror("pipe");
                return;
            }
        }

        pid_t pid = fork();
        if (pid < 0)
        {
            perror("fork");
            return;
        }

        if (pid == 0)
        {
            if (prev_read != -1)
            {
                dup2(prev_read, STDIN_FILENO);
                close(prev_read);
            }

            if (i < commands_count - 1)
            {
                close(pipefd[0]);
                dup2(pipefd[1], STDOUT_FILENO);
                close(pipefd[1]);
            }

            launch_process(commands[i]);
        }

        pids[i] = pid;

        if (prev_read != -1)
        {
            close(prev_read);
        }
        if (i < commands_count - 1)
        {
            close(pipefd[1]);
            prev_read = pipefd[0];
        }
    }
    for (int i = 0; i < commands_count; i++)
    {
        waitpid(pids[i], NULL, 0);
    }
   }
