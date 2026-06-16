#ifndef PROCESS_H
#define PROCESS_H

// Execute a single command without pipes.
void execute_simple_command(char *argv[]);

// Execute a pipeline of commands split by '|'.
void execute_pipe_command(char *argv[]);

// Parse the pipeline segments from the argv array.
int parse_pipeline(char *argv[], char **commands[]);

// Launch the pipeline child processes and manage pipes.
int launch_pipeline(char **commands[], int count);

#endif