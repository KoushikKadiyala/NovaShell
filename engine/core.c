#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "../include/parser.h"
#include "../include/executor.h"
#include "../include/builtins.h"
#include "../include/environment.h"
#include "../include/io.h"




int shell_execute(char *command) {
        char *argv[64];
        parse_input(command, argv);
        expand_variables(argv);

        if (argv[0] == NULL)
        {
            return 0;
        }

        if (strcmp(argv[0], "exit") == 0)
        {
            shell_print("Goodbye!\n");
            return 1;
        }

        if (handle_builtin(argv))
        {
            return 0;
        }
       
        execute_command(argv);
    return 0;   
}