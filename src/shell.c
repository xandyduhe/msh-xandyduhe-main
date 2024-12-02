// test 2.2
#include "shell.h"
#include <stdlib.h> 
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h> // issue with parsing and whitespace 
#include <unistd.h>    // For fork, execve
#include <sys/types.h> // For pid_t
#include <sys/wait.h>  // For waitpid
#include <errno.h>     // For perror

msh_t *shell = NULL;

//  initializes shell 
msh_t *alloc_shell(int max_jobs, int max_line, int max_history) {
    if (max_jobs == 0) max_jobs = DEFAULT_MAX_JOBS;
    if (max_line == 0) max_line = DEFAULT_MAX_LINE;
    if (max_history == 0) max_history = DEFAULT_MAX_HISTORY;

    msh_t *shell_state = malloc(sizeof(msh_t));
    if (!shell_state) return NULL;

    shell_state->max_jobs = max_jobs;
    shell_state->max_line = max_line;
    shell_state->max_history = max_history;

    shell_state->jobs = calloc(max_jobs, sizeof(job_t));
    if (!shell_state->jobs) {
        free(shell_state);
        return NULL;
    }

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

// Tokenizes the command line into individual jobs
char *parse_tok(char *line, int *job_type) {
    static char *current = NULL;

    if (line != NULL) {
        current = line;
    }

    if (!current || *current == '\0') {
        *job_type = -1;
        return NULL;
    }

    while (isspace((unsigned char)*current)) {
        current++;
    }

    if (*current == '\0') {
        *job_type = -1;
        return NULL;
    }

    char *start = current;

    while (*current && *current != ';' && *current != '&') {
        current++;
    }

    if (*current == ';') {
        *job_type = FOREGROUND;
        *current++ = '\0';
    } else if (*current == '&') {
        *job_type = BACKGROUND;
        *current++ = '\0';
    } else {
        *job_type = FOREGROUND;
    }

    char *end = current - 1;
    while (end > start && isspace((unsigned char)*end)) {
        *end-- = '\0';
    }

    return start;
}







// Separates a job into arguments and identifies built-in commands
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
            if (!argv) {
                for (int i = 0; i < *argc; i++) free(argv[i]);
                return NULL;
            }
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
    if (strlen(line) > shell->max_line) {
        fprintf(stderr, "error: reached the maximum line limit\n");
        return 0;
    }

    int job_type;
    char *job = parse_tok(line, &job_type);

    while (job && strlen(job) > 0) {
        int argc;
        bool is_builtin;
        char **argv = separate_args(job, &argc, &is_builtin);

        if (argv) {
            pid_t pid = fork();
            if (pid == -1) {
                perror("fork");
                free(argv);
                continue;
            }

            if (pid == 0) {
                execve(argv[0], argv, NULL);
                perror("execve");
                exit(EXIT_FAILURE);
            } else if (pid > 0) {
                if (job_type == FOREGROUND) {
                    add_job(shell->jobs, shell->max_jobs, pid, FOREGROUND, job);
                    int status;
                    waitpid(pid, &status, 0);
                    delete_job(shell->jobs, shell->max_jobs, pid);
                } else if (job_type == BACKGROUND) {
                    add_job(shell->jobs, shell->max_jobs, pid, BACKGROUND, job);
                }
            }
            free(argv);
        }
        job = parse_tok(NULL, &job_type);
    }

    return 0;
}



// free shell memory
void exit_shell(msh_t *shell) {
    for (int i = 0; i < shell->max_jobs; i++) {
        if (shell->jobs[i].state == BACKGROUND) {
            int status;
            waitpid(shell->jobs[i].pid, &status, 0);
            delete_job(shell->jobs, shell->max_jobs, shell->jobs[i].pid);
        }
    }

    free_jobs(shell->jobs, shell->max_jobs);
    free(shell);
}