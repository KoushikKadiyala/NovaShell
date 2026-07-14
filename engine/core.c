#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "../include/parser.h"
#include "../include/executor.h"
#include "../include/builtins.h"
#include "../include/environment.h"
#include "../include/io.h"
#include "../include/memory.h"


#define MAX_ARGS 64

int shell_execute(char *command) {
        char *argv[MAX_ARGS] = {NULL};
        parse_input(command, argv,MAX_ARGS);
        expand_variables(argv);

        if (argv[0] == NULL)
        {
            return 0;
        }

        if (strcmp(argv[0], "exit") == 0)
        {
            shell_print("Goodbye!\n");
            free_argv(argv);
            return 1;
        }

        if (handle_builtin(argv))
        {
            free_argv(argv);
            return 0;
        }
       
        execute_command(argv);
        free_argv(argv);
    return 0;   
}