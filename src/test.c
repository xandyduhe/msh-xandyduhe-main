#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include "shell.h"

// Parse command-line arguments
void parse_args(int argc, char *argv[], int *max_jobs, int *max_line, int *max_history) {
    int opt;
    while ((opt = getopt(argc, argv, "s:j:l:")) != -1) {
        switch (opt) {
            case 's':
                if (sscanf(optarg, "%d", max_history) != 1 || *max_history <= 0) {
                    fprintf(stderr, "usage: msh [-s NUMBER] [-j NUMBER] [-l NUMBER]\n");
                    exit(+1);
                }
                break;
            case 'j':
                if (sscanf(optarg, "%d", max_jobs) != 1 || *max_jobs <= 0) {
                    fprintf(stderr, "usage: msh [-s NUMBER] [-j NUMBER] [-l NUMBER]\n");
                    exit(1);
                }
                break;
            case 'l':
                if (sscanf(optarg, "%d", max_line) != 1 || *max_line <= 0) {
                    fprintf(stderr, "usage: msh [-s NUMBER] [-j NUMBER] [-l NUMBER]\n");
                    exit(1);
                }
                break;
            default:
                fprintf(stderr, "usage: msh [-s NUMBER] [-j NUMBER] [-l NUMBER]\n");
                exit(1);
        }
    }
}

// Interactive REPL loop
void repl_loop(msh_t *shell) {
    char *line = NULL;
    size_t len = 0;

    while (1) {
        ssize_t nread = getline(&line, &len, stdin);
        if (nread == -1) {
            free(line);
            break; // EOF or error
        }

        // Remove newline character
        line[strcspn(line, "\n")] = '\0';

        // Check line length before printing the prompt
        if (strlen(line) > shell->max_line) {
            printf("msh> error: reached the maximum line limit\n");
            continue; // Skip processing this input
        }

        // Print prompt only if the line length is valid
        printf("msh> ");
        if (evaluate(shell, line)) {
            free(line); // Free the line before breaking
            break;
        }
    }

    free(line);
}

void repl_loop(msh_t *shell) {
    char *line = NULL;
    size_t len = 0;

    while (1) {
        printf("msh> "); // Print prompt
        ssize_t nread = getline(&line, &len, stdin);
        if (nread == -1) {
            free(line);
            break; // EOF or error
        }

        // Remove newline character
        line[strcspn(line, "\n")] = '\0';

        // Evaluate the command line
        if (evaluate(shell, line)) {
            free(line); // Free the line before breaking
            break;
        }
    }
}


// Main entry point
int main(int argc, char *argv[]) {
    int max_jobs = 0, max_line = 0, max_history = 0;

    // Parse command-line arguments
    parse_args(argc, argv, &max_jobs, &max_line, &max_history);

    // Initialize shell state
    shell = alloc_shell(max_jobs, max_line, max_history);

    // Start REPL loop
    repl_loop(shell);

    // Clean up
    exit_shell(shell);
    return 0;
}


