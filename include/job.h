#ifndef _JOB_H_
#define _JOB_H_

#include <sys/types.h>
#include <stdbool.h>

typedef enum job_state { FOREGROUND, BACKGROUND, SUSPENDED, UNDEFINED } job_state_t;

// Represents a job in a shell.
typedef struct job {
    char *cmd_line;     // The command line for this specific job.
    job_state_t state;  // The current state for this job
    pid_t pid;          // The process id for this job
    int jid;            // The job number for this job
}job_t;

/*
 * add_job: Adds a job to the job array.
 * 
 * jobs: Array of job_t.
 * max_jobs: Maximum number of jobs in the array.
 * pid: Process ID of the job.
 * state: Job state (FOREGROUND/BACKGROUND).
 * cmd_line: Command line for the job.
 *
 * Returns: true if the job was added successfully; false otherwise.
 */
bool add_job(job_t *jobs, int max_jobs, pid_t pid, job_state_t state, const char *cmd_line);

/*
 * delete_job: Deletes a job by PID.
 * 
 * jobs: Array of job_t.
 * pid: PID of the job to delete.
 *
 * Returns: true if the job was deleted; false otherwise.
 */
bool delete_job(job_t *jobs, pid_t pid);

/*
 * free_jobs: Frees memory allocated for jobs.
 * 
 * jobs: Array of job_t.
 * max_jobs: Maximum number of jobs in the array.
 */
void free_jobs(job_t *jobs, int max_jobs);

#endif
