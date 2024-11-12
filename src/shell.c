// test 2.2
#include "shell.h"
#include <stdlib.h> 
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h> // issue with parsing and whitespace 

//  initializes shell 
msh_t *alloc_shell(int max_jobs, int max_line, int max_history) {

    // allocate memory for shell 
    msh_t *new_shell = (msh_t *)malloc(sizeof(msh_t));
    if (!new_shell) {
        return NULL; 
    }

    // limits
    new_shell->max_jobs = (max_jobs > 0) ? max_jobs : 16;
    new_shell->max_line = (max_line > 0) ? max_line : 1024;
    new_shell->max_history = (max_history > 0) ? max_history : 10;

    return new_shell;
}

// parses commands with &, ;
static char *current_line = NULL;

// check if string has ONLY whitespace
int white_space(const char *str) {
    while (*str) {
        if (!isspace((unsigned char)*str)) return 0;
        str++;
    }
    return 1;
}

char *parse_tok(char *line, int *job_type) {
    static char *current_line = NULL;

    if (line != NULL) {
        current_line = line;
        //printf("new line: '%s'\n", line);  
    }
    if (current_line == NULL || *current_line == '\0') {
        *job_type = -1;
        return NULL;
    }

    // find delimiter 
    char *next_job = current_line;
    char *delimiter_pos = strpbrk(current_line, "&;");

    if (delimiter_pos) {
        // determin job  
        *job_type = (*delimiter_pos == '&') ? 0 : 1;

        *delimiter_pos = '\0';
        current_line = delimiter_pos + 1;
        //printf("delititer, cmd: '%s', job: %d\n", next_job, *job_type);  
    } else {
        // no more delimiters
        *job_type = 1;  // foreground 
        current_line = NULL;
        //printf("last cmd: '%s', job: %d\n", next_job, *job_type);  
    }

    // skip whitespace commands
    if (white_space(next_job)) {
        return parse_tok(NULL, job_type);  
    }

    return next_job;

// executes command 
int evaluate(msh_t *shell, char *line) {
    if (strlen(line) > shell->max_line) {
        printf("error: max line limit\n");
        return 0;
    }

    int job_type;
    char *command = parse_tok(line, &job_type);
    while (command != NULL) {
        int argc;
        bool is_builtin;
        char **argv = separate_args(command, &argc, &is_builtin);

        // Print each argument
        for (int i = 0; i < argc; i++) {
            printf("argv[%d]=%s\n", i, argv[i]);
        }
        printf("argc=%d\n", argc);

        // Free the argument array
        for (int i = 0; i < argc; i++) {
            free(argv[i]);
        }
        free(argv);

        command = parse_tok(NULL, &job_type);
    }

    return 0; 
}

char **separate_args(char *line, int *argc, bool *is_builtin) {
    *argc = 0;
    *is_builtin = false;
    return NULL;
}

// free shell memory
void exit_shell(msh_t *shell) {
    if (shell) {
        free(shell); 
    }
}