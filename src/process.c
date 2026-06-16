#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>

#include "../include/process.h"
#include "../include/launcher.h"

#define MAX_COMMANDS 100

// Execute a single non-pipelined command using fork/exec.
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
        signal(SIGINT, SIG_DFL);
        launch_process(argv);
    }

    waitpid(pid, NULL, 0);
}

// Split argv into separate commands for pipeline execution.
int parse_pipeline(char *argv[], char **commands[]) {
    int count = 0;
    commands[count++] = argv;

    for (int i = 0; argv[i] != NULL; i++) {
        if (strcmp(argv[i], "|") == 0) {
            argv[i] = NULL;
            commands[count++] = &argv[i + 1];
        }
    }
    commands[count] = NULL;
    return count;
}

// Launch a pipeline of child processes, connecting STDOUT/STDIN with pipes.
int launch_pipeline(char **commands[], int count) {
    int prev_read = -1;
    pid_t pids[MAX_COMMANDS];

    for (int i = 0; i < count; i++) {
        int pipefd[2];
        if (i < count - 1) {
            if (pipe(pipefd) < 0) {
                perror("pipe");
                return -1;
            }
        }

        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            return -1;
        }

        if (pid == 0) {
            signal(SIGINT, SIG_DFL);
            if (prev_read != -1) {
                dup2(prev_read, STDIN_FILENO);
                close(prev_read);
            }
            if (i < count - 1) {
                close(pipefd[0]);
                dup2(pipefd[1], STDOUT_FILENO);
                close(pipefd[1]);
            }
            launch_process(commands[i]);
        } else {
            pids[i] = pid;
            if (prev_read != -1) {
                close(prev_read);
            }
            if (i < count - 1) {
                close(pipefd[1]);
                prev_read = pipefd[0];
            }
        }
    }

    for (int i = 0; i < count; i++) {
        waitpid(pids[i], NULL, 0);
    }
    return 0;
}

// Execute a parsed pipeline using the pipeline helper functions.
void execute_pipe_command(char *argv[]){
    char **commands[MAX_COMMANDS];

    int commands_count = parse_pipeline(argv, commands);

    launch_pipeline(commands, commands_count);
}