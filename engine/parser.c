#include <stdio.h>
#include <string.h>

#include "../include/parser.h"

void parse_input(char *input, char *argv[])
{
    char *p = input;
    int argc = 0;

    while (*p)
    {
        // Skip whitespace between tokens.
        while (*p == ' ' || *p == '\t')
        {
            p++;
        }

        if (*p == '\0')
            break;

        if (*p == '|')
        {
            argv[argc++] = "|";
            p++;
            continue;
        }

        if (*p == '<')
        {
            argv[argc++] = "<";
            p++;
            continue;
        }

        if (*p == '>')
        {
            if (*(p + 1) == '>')
            {
                argv[argc++] = ">>";
                p += 2;
            }
            else
            {
                argv[argc++] = ">";
                p++;
            }
            continue;
        }

        // Start a regular token.
        argv[argc++] = p;

        if (*p == '"' || *p == '\'')
        {
            char quote = *p;
            p++;
            argv[argc - 1] = p;

            while (*p != quote && *p != '\0')
            {
                p++;
            }

            if (*p == quote)
            {
                *p = '\0';
                p++;
            }
        }
        else
        {
            while (*p != '\0' && *p != ' ' && *p != '\t' && *p != '|' && *p != '<' && *p != '>')
            {
                p++;
            }

            if (*p == '\0')
                break;

            if (*p == ' ' || *p == '\t')
            {
                *p = '\0';
                p++;
            }
            else if (*p == '|')
            {
                *p = '\0';
                argv[argc++] = "|";
                p++;
            }
            else if (*p == '<')
            {
                *p = '\0';
                argv[argc++] = "<";
                p++;
            }
            else if (*p == '>')
            {
                *p = '\0';
                if (*(p + 1) == '>')
                {
                    argv[argc++] = ">>";
                    p += 2;
                }
                else
                {
                    argv[argc++] = ">";
                    p++;
                }
            }
        }
    }

    argv[argc] = NULL;
}