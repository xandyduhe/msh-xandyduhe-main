#include "job.h"
#include <stdlib.h>
#include <string.h>

bool add_job(job_t *jobs, int max_jobs, pid_t pid, job_state_t state, const char *cmd_line) {
    for (int i = 0; i < max_jobs; i++) {
        if (jobs[i].state == UNDEFINED) {
            jobs[i].pid = pid;
            jobs[i].state = state;
            jobs[i].jid = i + 1; // Assign a unique job ID
            jobs[i].cmd_line = strdup(cmd_line); // Duplicate the command line
            return true;
        }
    }
    return false; // No free slot
}

bool delete_job(job_t *jobs, int max_jobs, pid_t pid) {
    for (int i = 0; i < max_jobs; i++) {
        if (jobs[i].pid == pid) {
            free(jobs[i].cmd_line);
            jobs[i].state = UNDEFINED;
            jobs[i].pid = -1;
            jobs[i].jid = 0;
            return true;
        }
    }
    return false; // PID not found
}

void free_jobs(job_t *jobs, int max_jobs) {
    for (int i = 0; i < max_jobs; i++) {
        if (jobs[i].state != UNDEFINED) {
            free(jobs[i].cmd_line);
        }
    }
    free(jobs);
}
