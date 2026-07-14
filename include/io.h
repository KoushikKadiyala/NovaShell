#ifndef IO_H
#define IO_H

typedef void (*ShellOutputFn)(const char *);

void shell_set_output(ShellOutputFn printer);
void shell_print(const char *text);
void shell_printf(const char *format, ...);

#endif