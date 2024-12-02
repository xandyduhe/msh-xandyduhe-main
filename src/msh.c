#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include "shell.h"
#include <signal.h>
#include "job.h"
#include <sys/wait.h>

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
        printf("msh> ");
        ssize_t nread = getline(&line, &len, stdin);

        if (nread == -1) break;

        line[strcspn(line, "\n")] = '\0'; // Remove newline

        if (strlen(line) == 0) continue;
        if (strcmp(line, "exit") == 0) break;

        evaluate(shell, line);
    }

    free(line);
}


void handle_sigchld(int sig) {
    int status;
    pid_t pid;
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        delete_job(shell->jobs, shell->max_jobs, pid);
    }
}

void setup_signal_handlers() {
    struct sigaction sa;
    sa.sa_handler = handle_sigchld;
    sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
    sigaction(SIGCHLD, &sa, NULL);
}


int main(int argc, char *argv[]) {
    int max_jobs = 0, max_line = 0, max_history = 0;

    // Parse command-line arguments (original implementation retained)
    parse_args(argc, argv, &max_jobs, &max_line, &max_history);

    // Initialize shell state
    shell = alloc_shell(max_jobs, max_line, max_history);
    if (!shell) {
        fprintf(stderr, "error: unable to allocate memory for shell\n");
        exit(EXIT_FAILURE);
    }

    // Setup signal handlers
    setup_signal_handlers();

    // Run the REPL loop
    repl_loop(shell);

    // Cleanup and exit
    exit_shell(shell);
    return 0;
}