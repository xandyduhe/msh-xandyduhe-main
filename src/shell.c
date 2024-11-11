// teste

#include "shell.h"
#include <stdlib.h> 
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

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
    // Debug output to check inputs
    printf("Entering parse_tok with line='%s', current_line='%s'\n", line ? line : "NULL", current_line ? current_line : "NULL");

    // Check that job_type is not NULL
    if (job_type == NULL) {
        printf("Error: job_type is NULL\n");
        return NULL;
    }

    // Reset current_line if a new line is provided
    if (line != NULL) {
        current_line = line;
    }

    // If current_line is NULL or empty, indicate no more commands
    if (current_line == NULL || *current_line == '\0') {
        *job_type = -1;
        printf("No more tokens to parse. Exiting parse_tok.\n");
        return NULL;
    }

    // Find the next delimiter in the current line
    char *delimiter_pos = strpbrk(current_line, "&;");
    char *command_start = current_line;

    if (delimiter_pos != NULL) {
        *job_type = (*delimiter_pos == '&') ? 0 : 1;
        *delimiter_pos = '\0';  // Null-terminate at delimiter
        current_line = delimiter_pos + 1; // Move to the next part of the line
    } else {
        *job_type = 1; // Default to foreground job if no delimiter found
        current_line = NULL; // No more commands to parse
    }

    // Trim trailing whitespace only
    char *end = command_start + strlen(command_start) - 1;
    while (end > command_start && (*end == ' ' || *end == '\t')) {
        *end-- = '\0';
    }

    // If the command is empty after trimming, continue to the next token
    if (*command_start == '\0') {
        return parse_tok(NULL, job_type);
    }

    // Debug output to show the result
    printf("Returning command='%s', job_type=%d, remaining line='%s'\n",
           command_start, *job_type, current_line ? current_line : "NULL");

    return command_start;
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

char **separate_args(char *line, int *argc, bool *is_builtin) {
    // Stub implementation to satisfy the linker
    *argc = 0;
    *is_builtin = false;
    return NULL;
}

// Task 1: exit_shell - Deallocates the shell state memory
void exit_shell(msh_t *shell) {
    if (shell) {
        free(shell); // Free allocated memory
    }
}