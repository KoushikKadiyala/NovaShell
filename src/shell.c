#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "../include/shell.h"
#include "../include/parser.h"
#include "../include/executor.h"
#include "../include/builtins.h"

void start_shell() {
    char input[1024];
    char *argv[64];

    while (1) {
        char cwd[1024];
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("%s> ", cwd);
        } else {
            perror("getcwd");
            printf("$ ");
        }

        if (fgets(input, sizeof(input), stdin) == NULL) {
            break;
        }

        input[strcspn(input, "\n")] = '\0';

        parse_input(input, argv);

        if (argv[0] == NULL) {
    continue;
}

if (strcmp(argv[0], "exit") == 0) {
    printf("Goodbye!");
    break;
}

if (handle_builtin(argv)) {
    continue;
}

execute_command(argv);
       
    }
}