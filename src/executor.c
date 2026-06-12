#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

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
                printf("Syntax error: missing input file\n");
                _exit(1);
            }

            input_file = argv[i + 1];
            argv[i] = NULL;
        }

        else if (strcmp(argv[i], ">") == 0)
        {
            if (argv[i + 1] == NULL)
            {
                printf("Syntax error: missing output file\n");
                _exit(1);
            }

            output_file = argv[i + 1];
            argv[i] = NULL;
        }

        else if (strcmp(argv[i], ">>") == 0)
        {
            if (argv[i + 1] == NULL)
            {
                printf("Syntax error: missing output file\n");
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

    perror("execvp");
    _exit(1);
}

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

void execute_pipe_command(char *left_cmd[],
                          char *right_cmd[])
{
    int pipefd[2];

    if (pipe(pipefd) < 0)
    {
        perror("pipe");
        return;
    }

    pid_t pid1 = fork();

    if (pid1 == 0)
    {
        dup2(pipefd[1], STDOUT_FILENO);

        close(pipefd[0]);
        close(pipefd[1]);

        launch_process(left_cmd);
    }

    pid_t pid2 = fork();

    if (pid2 == 0)
    {
        dup2(pipefd[0], STDIN_FILENO);

        close(pipefd[0]);
        close(pipefd[1]);

        launch_process(right_cmd);
    }

    close(pipefd[0]);
    close(pipefd[1]);

    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);
}

void execute_command(char *argv[])
{
    int pipe_index = -1;

    for (int i = 0; argv[i] != NULL; i++)
    {
        if (strcmp(argv[i], "|") == 0)
        {
            pipe_index = i;
            break;
        }
    }

    if (pipe_index == -1)
    {
        execute_simple_command(argv);
        return;
    }

    if (pipe_index == 0)
    {
        printf("Syntax error before pipe\n");
        return;
    }

    if (argv[pipe_index + 1] == NULL)
    {
        printf("Syntax error after pipe\n");
        return;
    }

    argv[pipe_index] = NULL;

    char **left = argv;
    char **right = &argv[pipe_index + 1];

    execute_pipe_command(left, right);
}
