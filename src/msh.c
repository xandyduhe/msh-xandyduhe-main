#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include "shell.h"
#include <signal.h>
#include "job.h"
#include <sys/wait.h>

// print usage message and exit program
void print_usage_and_exit() {
    fprintf(stderr, "usage: msh [-s NUMBER] [-j NUMBER] [-l NUMBER]\n");
    fflush(stderr); // ensure immediate flushing to stderr
    exit(1);
}

// parse command-line arguments
void parse_args(int argc, char *argv[], int *max_jobs, int *max_line, int *max_history) {
    *max_jobs = 0;
    *max_line = 0;
    *max_history = 0;

    int opt;
    opterr = 0; // disable getopt's automatic error messages

    // parse arguments using getopt()
    while ((opt = getopt(argc, argv, ":s:j:l:")) != -1) {
        switch (opt) {
            case 's':
                // check if argument is valid for option 's'
                if (optarg == NULL || sscanf(optarg, "%d", max_history) != 1 || *max_history <= 0) {
                    print_usage_and_exit(); // exit immediately if invalid
                }
                break;

            case 'j':
                // check if argument is valid for option 'j'
                if (optarg == NULL || sscanf(optarg, "%d", max_jobs) != 1 || *max_jobs <= 0) {
                    print_usage_and_exit(); // exit immediately if invalid
                }
                break;

            case 'l':
                // check if argument is valid for option 'l'
                if (optarg == NULL || sscanf(optarg, "%d", max_line) != 1 || *max_line <= 0) {
                    print_usage_and_exit(); // exit immediately if invalid
                }
                break;

            case ':': // missing argument for option
                print_usage_and_exit(); // exit immediately if invalid
                break;

            case '?': // invalid option provided
                print_usage_and_exit(); // exit immediately if invalid
                break;

            default:
                print_usage_and_exit(); // exit immediately if invalid
        }
    }

    // check for unexpected extra arguments after options are parsed
    if (optind < argc) {
        print_usage_and_exit(); // exit immediately if extra arguments are found
    }
}

// interactive repl loop
void repl_loop(msh_t *shell) {
    char *line = NULL;
    size_t len = 0;

    while (1) {
        printf("msh> ");
        ssize_t nread = getline(&line, &len, stdin);

        if (nread == -1) break; // check if end of input is reached

        line[strcspn(line, "\n")] = '\0'; // remove newline character

        if (strlen(line) == 0) continue; // skip empty lines
        if (strcmp(line, "exit") == 0) break; // exit repl if "exit" command is given

        evaluate(shell, line); // evaluate command entered by user
    }

    free(line); // free memory allocated for line
}

// handle sigchld signal
void handle_sigchld(int sig) {
    int status;
    pid_t pid;
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        delete_job(shell->jobs, shell->max_jobs, pid); // delete completed job from job list
    }
}

// setup signal handlers
void setup_signal_handlers() {
    struct sigaction sa;
    sa.sa_handler = handle_sigchld; // set handler for sigchld
    sa.sa_flags = SA_RESTART | SA_NOCLDSTOP; // restart system calls and ignore stopped child processes
    sigaction(SIGCHLD, &sa, NULL); // apply sigchld handler
}

int main(int argc, char *argv[]) {
    int max_jobs = 0, max_line = 0, max_history = 0;

    // parse command-line arguments (original implementation retained)
    parse_args(argc, argv, &max_jobs, &max_line, &max_history);

    // initialize shell state
    shell = alloc_shell(max_jobs, max_line, max_history);
    if (!shell) {
        fprintf(stderr, "error: unable to allocate memory for shell\n");
        exit(EXIT_FAILURE);
    }

    // setup signal handlers
    setup_signal_handlers();

    // run repl loop
    repl_loop(shell);

    // cleanup and exit
    exit_shell(shell);
    return 0;
}
