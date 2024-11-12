#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "shell.h"

// pointer
msh_t *shell = NULL;

// print error message
void print_usage_and_exit() {
    fprintf(stderr, "usage: msh [-s NUMBER] [-j NUMBER] [-l NUMBER]\n");
    exit(1);
}

int main(int argc, char *argv[]) {
    int max_jobs = 0;
    int max_line = 0;
    int max_history = 0;
    
    // h: max history size; j: max num jobs; c: max num character in line
    int opt;
    while ((opt = getopt(argc, argv, "h:j:c:")) != -1) {
        int num;
        switch (opt) {
            case 'h':
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
            case 'c':
                if (sscanf(optarg, "%d", &num) != 1 || num <= 0) {
                    print_usage_and_exit();
                }
                max_line = num;
                break;
            default:
                print_usage_and_exit();
        }
    }

    // initialize shell 
    shell = alloc_shell(max_jobs, max_line, max_history);
    if (!shell) {
        fprintf(stderr, "Could not initialize \n");
        exit(1);
    }

    char *line = NULL;
    size_t len = 0;
    ssize_t nread;

    while (1) {
        //  prompt
        printf("msh> ");
        fflush(stdout);

        // Read from stdin
        nread = getline(&line, &len, stdin);
        if (nread == -1) {
            // fails -> exit loop
            break;
        }

        // remove newline char 
        if (line[nread - 1] == '\n') {
            line[nread - 1] = '\0';
        }

        // "exit" closes shell
        if (strcmp(line, "exit") == 0) {
            break;
        }

        evaluate(shell, line);
    }

    // free mem
    free(line); 
    exit_shell(shell);
    return 0;
}
