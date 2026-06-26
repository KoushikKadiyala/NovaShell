#include <stdio.h>
#include <string.h>
#include<stdlib.h>
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

static void skip_spaces(char **p){
    while (isspace((unsigned char)**p)){
    (*p)++;}
}

static char *parse_word(char **p){
    // measure length first
    char *scan = *p;
    size_t len = 0;
    while (*scan && !isspace((unsigned char)*scan) &&
           *scan != '|' && *scan != '<' && *scan != '>')
    {
        if (*scan == '\\' && *(scan+1)) { scan += 2; }
        else { scan++; }
        len++;
    }

    char *word = malloc(len + 1);
    char *dst = word;

    while (**p && !isspace((unsigned char)**p) &&
           **p != '|' && **p != '<' && **p != '>')
    {
        if ((**p == '\\') && *(*p + 1)){
            append_escaped_char(&dst, p);
        } else {
            *dst++ = *(*p)++;
        }
    }
    *dst = '\0';

    if (isspace((unsigned char)**p))
        (*p)++;

    return word;  // caller must free
}

static char *parse_quoted_string(char **p){
    char quote = **p;
    char *src = *p + 1;
    
    // measure length first
    char *scan = src;
    size_t len = 0;
    while (*scan && *scan != quote)
    {
        if (quote == '"' && *scan == '\\' && *(scan + 1))
            { scan += 2; }
        else if (quote == '\'' && *scan == '\\' && *(scan + 1) == '\'')
            { scan += 2; }
        else
            { scan++; }
        len++;
    }

    char *word = malloc(len + 1);
    char *dst = word;

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

    if (*src == quote)
        *p = src + 1;
    else
        *p = src;

    return word;
}
void parse_input(char *input, char *argv[], int MAX_ARGS)
{
    char *p = input;
    int argc = 0;

    while (*p != '\0')
    {
       skip_spaces(&p);

        if (*p == '\0')
            break;

        if(argc >= MAX_ARGS-1)
            break;
            

        const char *op = parse_operator(&p);
        if (op)
        {
            argv[argc++] = (char *)op;
            continue;
        }

        if (*p == '"' || *p == '\'')
        {
            argv[argc++] = parse_quoted_string(&p);
            continue;
        }
        argv[argc++] = parse_word(&p);
       
    }

    argv[argc] = NULL;
}
