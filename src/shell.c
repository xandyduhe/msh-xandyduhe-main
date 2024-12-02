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

    msh_t *shell_state = malloc(sizeof(msh_t));
    if (!shell_state) return NULL;

    shell_state->max_jobs = max_jobs;
    shell_state->max_line = max_line;
    shell_state->max_history = max_history;
    shell_state->jobs = calloc(max_jobs, sizeof(job_t)); // Allocate jobs array

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

    // Initialize on first call with new line
    if (line != NULL) {
        current = line;
    }

    // Return NULL if no more input or empty string
    if (!current || *current == '\0') {
        *job_type = -1;
        return NULL;
    }

    // Save start position (preserving leading whitespace)
    char *start = current;

    // Find the end of the command (stop at ; or &)
    char *end = start;
    while (*end && *end != ';' && *end != '&') {
        end++;
    }

    // Set job type based on delimiter
    if (*end == '&') {
        *job_type = 0;  // Background job
        *end = '\0';
        current = end + 1;
    } else if (*end == ';') {
        *job_type = 1;  // Foreground job
        *end = '\0';
        current = end + 1;
    } else {
        *job_type = 1;  // Default to foreground for last command
        current = end;  // Point to the null terminator
    }

    // If the remaining string is only whitespace, return NULL
    if (white_space(start)) {
        *job_type = -1;
        return NULL;
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
    int job_type;
    char *job = parse_tok(line, &job_type);

    while (job && strlen(job) > 0) {
        int argc;
        bool is_builtin;
        char **argv = separate_args(job, &argc, &is_builtin);

        if (argv) {
            pid_t pid = fork();
            if (pid == 0) {
                // Child process
                execve(argv[0], argv, NULL);
                perror("execve"); // Only reached if execve fails
                exit(1);
            } else if (pid > 0) {
                // Parent process
                if (job_type == FOREGROUND) {
                    add_job(shell->jobs, shell->max_jobs, pid, FOREGROUND, job);
                    int status;
                    waitpid(pid, &status, 0); // Wait for foreground job
                    delete_job(shell->jobs, pid);
                } else if (job_type == BACKGROUND) {
                    add_job(shell->jobs, shell->max_jobs, pid, BACKGROUND, job);
                }
            } else {
                perror("fork");
            }

            free(argv);
        }

        job = parse_tok(NULL, &job_type);
    }

    // Cleanup completed background jobs
    for (int i = 0; i < shell->max_jobs; i++) {
        if (shell->jobs[i].state == BACKGROUND) {
            int status;
            pid_t term_pid = waitpid(shell->jobs[i].pid, &status, WNOHANG);
            if (term_pid > 0) {
                delete_job(shell->jobs, term_pid);
            }
        }
    }

    return 0;
}


// free shell memory
void exit_shell(msh_t *shell) {
    for (int i = 0; i < shell->max_jobs; i++) {
        if (shell->jobs[i].state == BACKGROUND) {
            int status;
            waitpid(shell->jobs[i].pid, &status, 0);
            delete_job(shell->jobs, shell->jobs[i].pid);
        }
    }

    free_jobs(shell->jobs, shell->max_jobs);
    free(shell);
}
