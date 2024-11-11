#include "shell.h"
#include <stdlib.h> 
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

msh_t *shell = NULL; 

// Task 1: alloc_shell - Allocates and initializes the shell state
msh_t *alloc_shell(int max_jobs, int max_line, int max_history) {
    // Allocate memory for the shell state
    msh_t *new_shell = (msh_t *)malloc(sizeof(msh_t));
    if (!new_shell) {
        return NULL; // Allocation failed
    }

    // Set limits, using defaults if parameters are 0
    new_shell->max_jobs = (max_jobs > 0) ? max_jobs : 16;
    new_shell->max_line = (max_line > 0) ? max_line : 1024;
    new_shell->max_history = (max_history > 0) ? max_history : 10;

    // Initialize any additional fields if necessary

    return new_shell;
}

// Task 1: parse_tok - Parses commands separated by & or ;
static char *current_line = NULL;

char *parse_tok(char *line, int *job_type) {
    // If a new line is given, reset current_line
    if (line != NULL) {
        current_line = line;
    }

    // If no more tokens, return NULL
    if (current_line == NULL) {
        *job_type = -1;
        return NULL;
    }

    // Find the next delimiter
    char *delimiter_pos = strpbrk(current_line, "&;");
    char *command_start = current_line;

    // Determine job type based on delimiter
    if (delimiter_pos != NULL) {
        *job_type = (*delimiter_pos == '&') ? 0 : 1;
        *delimiter_pos = '\0';  // Null-terminate the command
        current_line = delimiter_pos + 1; // Move past delimiter
    } else {
        *job_type = 1; // Default to foreground job if no delimiter
        current_line = NULL; // No more commands to parse
    }

    return command_start;
}

// Task 1: separate_args - Separates a command line into arguments
char **separate_args(char *line, int *argc, bool *is_builtin) {
    if (line == NULL || strlen(line) == 0) {
        *argc = 0;
        return NULL;
    }

    // Count words in the line
    int arg_count = 0;
    char *token = strtok(line, " \t\n");
    while (token != NULL) {
        arg_count++;
        token = strtok(NULL, " \t\n");
    }

    // Allocate memory for arguments array
    char **argv = (char **)malloc((arg_count + 1) * sizeof(char *));
    if (!argv) {
        *argc = 0;
        return NULL;
    }

    // Split line into arguments and store in argv
    *argc = 0;
    token = strtok(line, " \t\n");
    while (token != NULL) {
        argv[*argc] = strdup(token); // Duplicate token to store
        (*argc)++;
        token = strtok(NULL, " \t\n");
    }
    argv[*argc] = NULL; // Null-terminate

    // Determine if the command is a built-in command (placeholder logic)
    *is_builtin = false; // For now, just set to false

    return argv;
}

// Task 1: evaluate - Executes the provided command line string
int evaluate(msh_t *shell, char *line) {
    if (strlen(line) > shell->max_line) {
        printf("error: reached the maximum line limit\n");
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

    return 0; // Return 0 to indicate shell should not exit
}

// Task 1: exit_shell - Deallocates the shell state memory
void exit_shell(msh_t *shell) {
    if (shell) {
        free(shell); // Free allocated memory
    }
}