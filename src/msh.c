#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "shell.h"

int main(int argc, char **argv) {
    int max_jobs = 0, max_line = 0, max_history = 0;

    // Process command-line arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-j") == 0 && i + 1 < argc) max_jobs = atoi(argv[++i]);
        else if (strcmp(argv[i], "-l") == 0 && i + 1 < argc) max_line = atoi(argv[++i]);
        else if (strcmp(argv[i], "-s") == 0 && i + 1 < argc) max_history = atoi(argv[++i]);
        else {
            fprintf(stderr, "usage: msh [-s NUMBER] [-j NUMBER] [-l NUMBER]\n");
            return 1;
        }
    }

    shell = alloc_shell(max_jobs, max_line, max_history);
    if (!shell) {
        fprintf(stderr, "Failed to allocate shell state\n");
        return 1;
    }

    char *line = NULL;
    size_t len = 0;

    while (1) {
        // Print the prompt
        printf("msh> ");
        if (getline(&line, &len, stdin) == -1) break; // Handle EOF

        // Trim and check for empty input
        char *trimmed = line;
        while (*trimmed == ' ' || *trimmed == '\t') trimmed++;
        if (*trimmed == '\0') continue; // Skip empty input

        // Exit condition
        if (strcmp(trimmed, "exit") == 0) break;

        // Evaluate the command
        evaluate(shell, trimmed);
    }

    free(line);
    exit_shell(shell);

    // Print a final prompt without newline
    printf("msh>");
    return 0;
}



