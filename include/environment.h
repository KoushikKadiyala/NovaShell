#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

// Expand environment variable tokens such as $HOME in command arguments.
void expand_variables(char *argv[]);

// Placeholder for any future environment-related builtin commands.
int handle_environment_command(char *argv[]);

#endif