#include "signal_handlers.h"
#include "shell.h"
#include "job.h"
#include <signal.h>
#include <sys/wait.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>

extern msh_t *shell; // Access the global shell state

/*
 * sigchld_handler - The kernel sends a SIGCHLD to the shell whenever
 *     a child job terminates (becomes a zombie), or stops because it
 *     received a SIGSTOP or SIGTSTP signal. The handler reaps all
 *     available zombie children, but doesn't wait for any other
 *     currently running children to terminate.
 */
void sigchld_handler(int sig) {
    int status;
    pid_t pid;

    // Reap all available zombie children
    while ((pid = waitpid(-1, &status, WNOHANG | WUNTRACED | WCONTINUED)) > 0) {
        if (WIFEXITED(status)) {
            printf("DEBUG: Child process (PID: %d) exited normally.\n", pid);
            delete_job(shell->jobs, shell->max_jobs, pid);
        } else if (WIFSIGNALED(status)) {
            printf("DEBUG: Child process (PID: %d) terminated by signal %d.\n", pid, WTERMSIG(status));
            delete_job(shell->jobs, shell->max_jobs, pid);
        } else if (WIFSTOPPED(status)) {
            printf("DEBUG: Child process (PID: %d) stopped by signal %d.\n", pid, WSTOPSIG(status));
            job_t *job = get_job_by_pid(shell->jobs, shell->max_jobs, pid);
            if (job) {
                job->state = SUSPENDED;
            }
        } else if (WIFCONTINUED(status)) {
            printf("DEBUG: Child process (PID: %d) continued.\n", pid);
            job_t *job = get_job_by_pid(shell->jobs, shell->max_jobs, pid);
            if (job) {
                job->state = BACKGROUND;
            }
        }
    }

    if (pid == -1 && errno != ECHILD) {
        perror("waitpid");
    }
}

/*
 * sigint_handler - The kernel sends a SIGINT to the shell whenever the
 *    user types ctrl-c at the keyboard. Catch it and send it along
 *    to the foreground job.
 */
void sigint_handler(int sig) {
    printf("DEBUG: Caught SIGINT (Ctrl+C).\n");

    // Find the foreground job and send it the SIGINT signal
    job_t *fg_job = get_foreground_job(shell->jobs, shell->max_jobs);
    if (fg_job) {
        printf("DEBUG: Sending SIGINT to foreground job (PID: %d).\n", fg_job->pid);
        if (kill(-fg_job->pid, SIGINT) == -1) {
            perror("kill");
        }
    }
}

/*
 * sigtstp_handler - The kernel sends a SIGTSTP to the shell whenever
 *     the user types ctrl-z at the keyboard. Catch it and suspend the
 *     foreground job by sending it a SIGTSTP signal.
 */
void sigtstp_handler(int sig) {
    printf("DEBUG: Caught SIGTSTP (Ctrl+Z).\n");

    // Find the foreground job and send it the SIGTSTP signal
    job_t *fg_job = get_foreground_job(shell->jobs, shell->max_jobs);
    if (fg_job) {
        printf("DEBUG: Sending SIGTSTP to foreground job (PID: %d).\n", fg_job->pid);
        if (kill(-fg_job->pid, SIGTSTP) == -1) {
            perror("kill");
        }
    }
}

/*
 * setup_handler - Wrapper for the sigaction function
 *
 * Citation: Bryant and O’Hallaron, Computer Systems: A Programmer’s Perspective, Third Edition
 */
typedef void handler_t(int);

handler_t *setup_handler(int signum, handler_t *handler) {
    struct sigaction action, old_action;

    action.sa_handler = handler;
    sigemptyset(&action.sa_mask); /* Block signals of type being handled */
    action.sa_flags = SA_RESTART; /* Restart system calls if possible */

    if (sigaction(signum, &action, &old_action) < 0) {
        perror("sigaction");
        exit(1);
    }

    return (old_action.sa_handler);
}

/*
 * initialize_signal_handlers - Sets up all the signal handlers for the shell
 */
void initialize_signal_handlers() {
    setup_handler(SIGCHLD, sigchld_handler); /* Handles child process events */
    setup_handler(SIGINT, sigint_handler);  /* Handles Ctrl+C */
    setup_handler(SIGTSTP, sigtstp_handler); /* Handles Ctrl+Z */
}
