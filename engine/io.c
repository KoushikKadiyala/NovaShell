#include "../include/io.h"

#include <stdio.h>
#include <stdarg.h>

static ShellOutputFn printer = NULL;

void shell_set_output(ShellOutputFn fn)
{
    printer =fn;
}

void shell_print(const char *text)
{
    if (printer)
    {
        printer(text);
    }
    else
    {
        fputs(text, stdout);
        fflush(stdout);
    }
}

void shell_printf(const char *format, ...){
    char buffer[4096];

    va_list args;
    va_start(args,format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    shell_print(buffer);
}