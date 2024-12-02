#include "job.h"
#include <stdlib.h>
#include <string.h>

// adds job to job list
bool add_job(job_t *jobs, int max_jobs, pid_t pid, job_state_t state, const char *cmd_line) {
    for (int i = 0; i < max_jobs; i++) {
        if (jobs[i].state == UNDEFINED) { // find first undefined job slot
            jobs[i].pid = pid; // set job pid
            jobs[i].state = state; // set job state
            jobs[i].jid = i + 1; // assign job id
            jobs[i].cmd_line = strdup(cmd_line); // duplicate command line
            return true; // return true if job added successfully
        }
    }
    return false; // return false if no available slot for job
}

// deletes job from job list
bool delete_job(job_t *jobs, int max_jobs, pid_t pid) {
    for (int i = 0; i < max_jobs; i++) {
        if (jobs[i].pid == pid) { // find job with matching pid
            free(jobs[i].cmd_line); // free command line memory
            jobs[i].state = UNDEFINED; // set job state to undefined
            jobs[i].pid = -1; // reset pid
            jobs[i].jid = 0; // reset job id
            return true; // return true if job deleted successfully
        }
    }
    return false; // return false if job not found
}

// frees memory allocated for jobs
void free_jobs(job_t *jobs, int max_jobs) {
    for (int i = 0; i < max_jobs; i++) {
        if (jobs[i].state != UNDEFINED) { // check if job is defined
            free(jobs[i].cmd_line); // free command line memory
        }
    }
    free(jobs); // free jobs array
}
