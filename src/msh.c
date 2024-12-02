#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include "shell.h"


void print_usage_and_exit() {
    fprintf(stderr, "usage: msh [-s NUMBER] [-j NUMBER] [-l NUMBER]\n");
    fflush(stderr); // Ensure immediate flushing to stderr
    exit(1);
}

// Parse command-line arguments
void parse_args(int argc, char *argv[], int *max_jobs, int *max_line, int *max_history) {
    *max_jobs = 0;
    *max_line = 0;
    *max_history = 0;

    int opt;
    opterr = 0; // Disable getopt's automatic error messages

    // Parse the arguments using getopt()
    while ((opt = getopt(argc, argv, ":s:j:l:")) != -1) {
        switch (opt) {
            case 's':
                if (optarg == NULL || sscanf(optarg, "%d", max_history) != 1 || *max_history <= 0) {
                    print_usage_and_exit(); // Exit immediately if invalid
                }
                break;

            case 'j':
                if (optarg == NULL || sscanf(optarg, "%d", max_jobs) != 1 || *max_jobs <= 0) {
                    print_usage_and_exit(); // Exit immediately if invalid
                }
                break;

            case 'l':
                if (optarg == NULL || sscanf(optarg, "%d", max_line) != 1 || *max_line <= 0) {
                    print_usage_and_exit(); // Exit immediately if invalid
                }
                break;

            case ':': // Missing argument for an option
                print_usage_and_exit(); // Exit immediately if invalid
                break;

            case '?': // Invalid option provided
                print_usage_and_exit(); // Exit immediately if invalid
                break;

            default:
                print_usage_and_exit(); // Exit immediately if invalid
        }
    }

    // Check for unexpected extra arguments after the options are parsed
    if (optind < argc) {
        print_usage_and_exit(); // Exit immediately if extra arguments are found
    }
}

// Interactive REPL loop
void repl_loop(msh_t *shell) {
    char *line = NULL;
    size_t len = 0;

    while (1) {
        printf("msh> "); // Print prompt
        ssize_t nread = getline(&line, &len, stdin);
        if (nread == -1) {
            free(line);
            line = NULL; // Safeguard: Prevent invalid free
            break; // EOF or error
        }

        // Remove newline character
        line[strcspn(line, "\n")] = '\0';

        // Check line length before processing
        if (strlen(line) > shell->max_line) {
            printf("error: reached the maximum line limit\n");
            continue; // Skip this input
        }

        // Evaluate the command line
        if (evaluate(shell, line)) {
            free(line);
            line = NULL; // Safeguard: Prevent invalid free
            break;
        }
    }

    free(line); // Free any remaining buffer
    line = NULL; // Safeguard: Prevent invalid free
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