#ifndef EXECUTOR_H
#define EXECUTOR_H

// Dispatch a parsed command to either built-in, simple, or pipeline execution.
void execute_command(char *argv[]);

#endif