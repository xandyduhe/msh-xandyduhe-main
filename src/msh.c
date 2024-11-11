#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "shell.h"

#define _GNU_SOURCE

// Global shell state pointer
msh_t *shell = NULL;

// Helper function to print usage message and exit with error
void print_usage_and_exit() {
    fprintf(stderr, "usage: msh [-s NUMBER] [-j NUMBER] [-l NUMBER]\n");
    exit(1);
}

int main(int argc, char *argv[]) {
    int max_jobs = 0;
    int max_line = 0;
    int max_history = 0;
    
    // Command-line argument parsing
    int opt;
    while ((opt = getopt(argc, argv, "s:j:l:")) != -1) {
        int num;
        switch (opt) {
            case 's':
                if (sscanf(optarg, "%d", &num) != 1 || num <= 0) {
                    print_usage_and_exit();
                }
                max_history = num;
                break;
            case 'j':
                if (sscanf(optarg, "%d", &num) != 1 || num <= 0) {
                    print_usage_and_exit();
                }
                max_jobs = num;
                break;
            case 'l':
                if (sscanf(optarg, "%d", &num) != 1 || num <= 0) {
                    print_usage_and_exit();
                }
                max_line = num;
                break;
            default:
                print_usage_and_exit();
        }
    }

    // Allocate and initialize shell state
    shell = alloc_shell(max_jobs, max_line, max_history);
    if (!shell) {
        fprintf(stderr, "Failed to initialize shell.\n");
        exit(1);
    }

    // Main REPL loop
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;

    while (1) {
        // Display the prompt
        printf("msh> ");
        fflush(stdout);

        // Read a line from standard input
        nread = getline(&line, &len, stdin);
        if (nread == -1) {
            // If getline fails, exit loop
            break;
        }

        // Remove newline character from the end of line
        if (line[nread - 1] == '\n') {
            line[nread - 1] = '\0';
        }

        // Check if the user entered "exit" to close the shell
        if (strcmp(line, "exit") == 0) {
            break;
        }

        // Evaluate the command line
        evaluate(shell, line);
    }

    // Cleanup
    free(line); // Free the line buffer
    exit_shell(shell); // Free shell resources
    return 0;
}
