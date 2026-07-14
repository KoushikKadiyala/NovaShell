#include "../include/shell.h"
#include "../include/io.h"

#include <stdio.h>

static void console_output(const char *text){
    fputs(text, stdout);
    fflush(stdout);
}

// Entry point for the shell executable.
int main() {
    shell_set_output(console_output);
    start_shell();
    return 0;
}