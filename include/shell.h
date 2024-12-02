#ifndef _SHELL_H_
#define _SHELL_H_

#include <stdbool.h>
#include "job.h"

// Default values for shell configuration
#define DEFAULT_MAX_JOBS 16
#define DEFAULT_MAX_LINE 1024
#define DEFAULT_MAX_HISTORY 10
 
// Represents the state of the shell 
typedef struct msh {
    /** TODO: IMPLEMENT **/ 
    int max_jobs; // max jobs allowed
    int max_line;  // max characters/ line
    int max_history; // maz commands in history
    job_t *jobs; // Array of jobs
} msh_t;


extern msh_t *shell;

/*
* alloc_shell: allocates and initializes the state of the shell
*
* max_jobs: The maximum number of jobs that can be in existence at any point in time.
*
* max_line: The maximum number of characters that can be entered for any specific command line.
*
* max_history: The maximum number of saved history commands for the shell.
*
* Returns: a msh_t pointer that is allocated and initialized
*/
msh_t *alloc_shell(int max_jobs, int max_line, int max_history);

/**
* parse_tok: Continuously retrieves separate commands from the provided command line until all commands are parsed
*
* line:  the command line to parse, which may include multiple commands. If line is NULL then parse_tok continues parsing the previous command line.
*
* job_type: Specifies whether the parsed command is a background (sets the value of 0 at the address of job_type) or foreground job (sets the value of 1 at the address of job_type). If no job is returned then assign the value at the address to -1
*
* Returns: NULL no other commands can be parsed; otherwise, it returns a parsed command from the command line.
*
* Please note this function does modify the ``line`` parameter.
*/
char *parse_tok(char *line, int *job_type);

/**
* separate_args: Separates the arguments of command and places them in an allocated array returned by this function
*
* line: the command line to separate. This function assumes only a single command that takes in zero or more arguments.
*
* argc: Stores the number of arguments produced at the memory location of the argc pointer.
*
* is_builtin: true if the command is a built-in command; otherwise false.
*
* Returns: NULL is line contains no arguments; otherwise, a newly allocated array of strings that represents the arguments of the command (similar to argv). Make sure the array includes a NULL value in its last location.
* Note: The user is responsible for freeing the memory return by this function!
*/
char **separate_args(char *line, int *argc, bool *is_builtin);

/*
* evaluate - executes the provided command line string
*
* shell - the current shell state value
*
* line - the command line string to evaluate
*
* Returns: non-zero if the command executed wants the shell program to close. Otherwise, a 0 is returned.
*/
int evaluate(msh_t *shell, char *line);

//add whitespace declaration 
int white_space(const char *str); // Add this declaration

/*
* exit_shell - Closes down the shell by deallocating the shell state.
*
* shell - the current shell state value
*
*/
void exit_shell(msh_t *shell);

#endif 