#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "../include/parser.h"

static const char *parse_operator(char **p)
{
    if (**p == '|')
    {
        (*p)++;
        return "|";
    }

    if (**p == '<')
    {
        (*p)++;
        return "<";
    }

    if (**p == '>')
    {
        if (*((*p) + 1) == '>')
        {
            *p += 2;
            return ">>";
        }

        (*p)++;
        return ">";
    }

    return NULL;
}

static void append_escaped_char(char **dst, char **src)
{
    (*src)++;
    if (**src != '\0')
    {
        **dst = **src;
        (*dst)++;
        (*src)++;
    }
}

void parse_input(char *input, char *argv[])
{
    char *p = input;
    int argc = 0;

    while (*p)
    {
        while (isspace((unsigned char)*p))
        {
            *p = '\0';
            p++;
        }

        if (*p == '\0')
            break;

        const char *op = parse_operator(&p);
        if (op != NULL)
        {
            argv[argc++] = (char *)op;
            continue;
        }

        argv[argc++] = p;

        if (*p == '"' || *p == '\'')
        {
            char quote = *p;
            char *src = p + 1;
            char *dst = p;
            argv[argc - 1] = dst;

            while (*src && *src != quote)
            {
                if (quote == '"' && *src == '\\' && *(src + 1))
                {
                    append_escaped_char(&dst, &src);
                }
                else if (quote == '\'' && *src == '\\' && *(src + 1) == '\'')
                {
                    append_escaped_char(&dst, &src);
                }
                else
                {
                    *dst++ = *src++;
                }
            }

            *dst = '\0';
            p = src;
            if (*p == quote)
                p++;
        }
        else
        {
            char *src = p;
            char *dst = p;

            while (*src && !isspace((unsigned char)*src) && *src != '|' && *src != '<' && *src != '>')
            {
                if (*src == '\\' && *(src + 1))
                {
                    append_escaped_char(&dst, &src);
                }
                else
                {
                    *dst++ = *src++;
                }
            }
            char sep = *src;
            *dst = '\0';
            p = src;

            if (sep == '\0')
                break;

            if (sep == '|' || sep == '<' || sep == '>')
            {
                const char *op = parse_operator(&p);
                if (op != NULL)
                {
                    argv[argc++] = (char *)op;
                }
            }
            else if (isspace((unsigned char)sep))
            {
                p++;
            }
        }
    }

    argv[argc] = NULL;
}
