#include<stdio.h>
#include<string.h>

// Tokenize the input string into argv[] using spaces as separators.
// This simple parser does not currently support quoted arguments.
void parse_input(char *input, char *argv[]){
    int i = 0;
    char *token = strtok(input, " ");

    while (token != NULL) {
        argv[i] = token;
        i++;
        token = strtok(NULL, " ");
    }
    argv[i] = NULL;
}