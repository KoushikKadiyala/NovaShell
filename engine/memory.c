#include <stdlib.h>
#include <string.h>

#include "../include/memory.h"

static int is_operator(const char *token)
{
    return strcmp(token, "|")  == 0 ||
           strcmp(token, "<")  == 0 ||
           strcmp(token, ">")  == 0 ||
           strcmp(token, ">>") == 0;
}

void free_argv(char *argv[])
{
    for (int i = 0; argv[i] != NULL; i++)
    {
        if (!is_operator(argv[i]))
        {
            free(argv[i]);
        }
    }
}