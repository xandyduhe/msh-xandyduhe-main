#ifndef _JOB_H_
#define _JOB_H_

#include <sys/types.h>
#include <stdbool.h>

typedef enum job_state { FOREGROUND, BACKGROUND, SUSPENDED, UNDEFINED } job_state_t;

typedef struct job {
    char *cmd_line;     // The command line for this specific job.
    job_state_t state;  // The current state for this job
    pid_t pid;          // The process id for this job
    int jid;            // The job number for this job
} job_t;

job_t *get_job_by_pid(job_t *jobs, int max_jobs, pid_t pid);
job_t *get_foreground_job(job_t *jobs, int max_jobs);

// adds job to job list
bool add_job(job_t *jobs, int max_jobs, pid_t pid, job_state_t state, const char *cmd_line);

// deletes job from job list
bool delete_job(job_t *jobs, int max_jobs, pid_t pid); // updated with max_jobs

// frees memory allocated for jobs
void free_jobs(job_t *jobs, int max_jobs);

#endif

