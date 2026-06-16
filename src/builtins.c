#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#define PATH_MAX 4096

// Remember the previous working directory for `cd -` behavior.
static char previous_dir[PATH_MAX] = "";

// Handle builtin commands. Currently supports `cd`.
int handle_builtin(char *argv[]) {

    if (strcmp(argv[0], "cd") == 0) {

        char *target;
        char current_dir[PATH_MAX];

        if (getcwd(current_dir, sizeof(current_dir)) == NULL) {
            perror("getcwd");
            return 1;
        }

        if (argv[1] == NULL || strcmp(argv[1], "~") == 0) {

            target = getenv("HOME");

            if (target == NULL) {
                printf("cd: HOME not set\n");
                return 1;
            }
        }
        else if (strcmp(argv[1], "-") == 0) {

            if (previous_dir[0] == '\0') {
                printf("cd: OLDPWD not set\n");
                return 1;
            }

            target = previous_dir;
            printf("%s\n", target);
        }
        else {

            target = argv[1];
        }

        if (chdir(target) != 0) {
            perror("cd");
            return 1;
        }

        strcpy(previous_dir, current_dir);

        return 1;
    }

    return 0;
}