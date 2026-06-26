#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <string.h>

#include "../include/environment.h"

extern char **environ;

// Replace argv tokens starting with '$' by their environment values.
// This only supports whole-token variable expansion, such as `$HOME`.
void expand_variables(char *argv[]) {
    for (int i = 0; argv[i] != NULL; i++) {
        if (argv[i][0] == '$') {
            char *var_name = argv[i] + 1;
            char *value = getenv(var_name);

            if (value != NULL)
            {
                argv[i] = strdup(value);
            }
            else {
                argv[i] = strdup("");
            }
        }
    }
}
