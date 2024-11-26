// test 2.2
#include "shell.h"
#include <stdlib.h> 
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h> // issue with parsing and whitespace 

msh_t *shell = NULL;

//  initializes shell 
msh_t *alloc_shell(int max_jobs, int max_line, int max_history) {
    const int MAX_LINE = 1024;
    const int MAX_JOBS = 16;
    const int MAX_HISTORY = 10;

    if (max_jobs == 0) max_jobs = MAX_JOBS;
    if (max_line == 0) max_line = MAX_LINE;
    if (max_history == 0) max_history = MAX_HISTORY;

    // allocate memory for shell 
    msh_t *shell_state = malloc(sizeof(msh_t));
    if (!shell_state) return NULL;

    // limits
    shell_state->max_jobs = max_jobs;
    shell_state->max_line = max_line;
    shell_state->max_history = max_history;

    return shell_state;
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
    static char *current = NULL;

    if (line != NULL) current = line;

    if (!current || *current == '\0') {
        *job_type = -1;
        return NULL;
    }

    // Skip leading delimiters and whitespace
    while (*current == ' ' || *current == ';' || *current == '&') current++;

    if (*current == '\0') {
        *job_type = -1;
        return NULL;
    }

    char *start = current;
    while (*current && *current != ';' && *current != '&') current++;

    if (*current == ';') {
        *job_type = 1; // Foreground job
        *current++ = '\0';
    } else if (*current == '&') {
        *job_type = 0; // Background job
        *current++ = '\0';
    } else {
        *job_type = 1; // Treat as foreground job
    }

    return start;
}



char **separate_args(char *line, int *argc, bool *is_builtin) {
    if (!line || !*line) {
        *argc = 0;
        return NULL;
    }

    int capacity = 10;
    char **argv = malloc(capacity * sizeof(char *));
    if (!argv) return NULL;

    *argc = 0;
    char *token = strtok(line, " \t");
    while (token) {
        if (*argc >= capacity) {
            capacity *= 2;
            argv = realloc(argv, capacity * sizeof(char *));
            if (!argv) return NULL;
        }
        argv[(*argc)++] = token;
        token = strtok(NULL, " \t");
    }

    argv[*argc] = NULL;
    *is_builtin = false; 
    return argv;
}




// executes command 
int evaluate(msh_t *shell, char *line) {
    // Trim leading and trailing whitespace
    while (*line == ' ' || *line == '\t') line++;
    if (*line == '\0') return 0; // Ignore empty input

    if (strlen(line) > shell->max_line) {
        fprintf(stderr, "error: reached the maximum line limit\n");
        return 0;
    }

    int job_type;
    char *job = parse_tok(line, &job_type);
    while (job) {
        int argc;
        bool is_builtin;
        char **argv = separate_args(job, &argc, &is_builtin);

        if (argv) {
            for (int i = 0; i < argc; i++) {
                printf("argv[%d]=%s\n", i, argv[i]);
            }
            printf("argc=%d\n", argc);
            free(argv);
        }

        job = parse_tok(NULL, &job_type);
    }

    return 0;
}



// free shell memory
void exit_shell(msh_t *shell) {
    if (shell) {
        free(shell); 
    }
}