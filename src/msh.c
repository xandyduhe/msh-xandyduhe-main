#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include "shell.h"

// Parse command-line arguments
void parse_args(int argc, char *argv[], int *max_jobs, int *max_line, int *max_history) {
    *max_jobs = 0;
    *max_line = 0;
    *max_history = 0;

    int opt;
    int invalid_usage = 0;

    while ((opt = getopt(argc, argv, "s:j:l:")) != -1) {
        switch (opt) {
            case 's':
                if (sscanf(optarg, "%d", max_history) != 1 || *max_history <= 0) {
                    invalid_usage = 1;
                }
                break;
            case 'j':
                if (sscanf(optarg, "%d", max_jobs) != 1 || *max_jobs <= 0) {
                    invalid_usage = 1;
                }
                break;
            case 'l':
                if (sscanf(optarg, "%d", max_line) != 1 || *max_line <= 0) {
                    invalid_usage = 1;
                }
                break;
            default:
                invalid_usage = 1;
        }

        // If an invalid usage flag is set, terminate further checks early
        if (invalid_usage) break;
    }

    // Final check for invalid usage
    if (invalid_usage || optind < argc) {
        fprintf(stderr, "usage: msh [-s NUMBER] [-j NUMBER] [-l NUMBER]\n");
        fflush(stderr);
        exit(1);
    }
}





// Interactive REPL loop
void repl_loop(msh_t *shell) {
    char *line = NULL;
    size_t len = 0;

    while (1) {
        printf("msh> ");
        ssize_t nread = getline(&line, &len, stdin);
        if (nread == -1) {
            free(line);
            break; // EOF or error
        }

        // Remove newline character
        line[strcspn(line, "\n")] = 0;

        if (strcmp(line, "exit") == 0) {
            free(line);
            break;
        }

        // Pass the command line to `evaluate`
        evaluate(shell, line);
    }
}

// Main entry point
int main(int argc, char *argv[]) {
    int max_jobs = 0, max_line = 0, max_history = 0;

    // Parse command-line arguments
    parse_args(argc, argv, &max_jobs, &max_line, &max_history);

    // Initialize shell state
    shell = alloc_shell(max_jobs, max_line, max_history);

    if (shell == NULL) {
        fprintf(stderr, "error: unable to allocate memory for shell\n");
        exit(1);
    }

    // Start REPL loop
    repl_loop(shell);

    // Clean up
    exit_shell(shell);
    return 0;
}

