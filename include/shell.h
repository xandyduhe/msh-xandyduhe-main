#ifndef _SHELL_H_
#define _SHELL_H_

#include <stdbool.h>
#include "job.h"          // For job_t definitions
#include "history.h"      // For history_t definitions

// Default values for shell configuration
#define DEFAULT_MAX_JOBS 16
#define DEFAULT_MAX_LINE 1024
#define DEFAULT_MAX_HISTORY 10

// Represents the state of the shell
typedef struct msh {
    int max_jobs;         // Maximum number of jobs allowed
    int max_line;         // Maximum characters per line
    int max_history;      // Maximum commands in history
    job_t *jobs;          // Array of jobs
    history_t *history;   // Shell history structure
} msh_t;

extern msh_t *shell;

/*
 * alloc_shell: allocates and initializes the state of the shell
 *
 * max_jobs: The maximum number of jobs that can be in existence at any point in time.
 * max_line: The maximum number of characters that can be entered for any specific command line.
 * max_history: The maximum number of saved history commands for the shell.
 *
 * Returns: a msh_t pointer that is allocated and initialized
 */
msh_t *alloc_shell(int max_jobs, int max_line, int max_history);

/*
 * parse_tok: Continuously retrieves separate commands from the provided command line until all commands are parsed
 *
 * line:  the command line to parse, which may include multiple commands. If line is NULL then parse_tok continues parsing the previous command line.
 * job_type: Specifies whether the parsed command is a background (sets the value of 0 at the address of job_type) or foreground job (sets the value of 1 at the address of job_type). If no job is returned then assign the value at the address to -1.
 *
 * Returns: NULL no other commands can be parsed; otherwise, it returns a parsed command from the command line.
 *
 * Note: This function modifies the `line` parameter.
 */
char *parse_tok(char *line, int *job_type);

char *builtin_cmd(int argc, char **argv);

/*
 * separate_args: Separates the arguments of command and places them in an allocated array returned by this function.
 *
 * line: the command line to separate. This function assumes only a single command that takes in zero or more arguments.
 * argc: Stores the number of arguments produced at the memory location of the argc pointer.
 * is_builtin: true if the command is a built-in command; otherwise false.
 *
 * Returns: NULL if `line` contains no arguments; otherwise, a newly allocated array of strings that represents the arguments of the command (similar to argv). Make sure the array includes a NULL value in its last location.
 *
 * Note: The user is responsible for freeing the memory returned by this function!
 */
char **separate_args(char *line, int *argc, bool *is_builtin);

/*
 * evaluate: Executes the provided command line string.
 *
 * shell: The current shell state value.
 *
 * line: The command line string to evaluate.
 *
 * Returns: Non-zero if the command executed wants the shell program to close. Otherwise, a 0 is returned.
 */
int evaluate(msh_t *shell, char *line);

/*
 * white_space: Determines whether a string contains only whitespace characters.
 *
 * str: The string to check.
 *
 * Returns: 1 if the string contains only whitespace; 0 otherwise.
 */
int white_space(const char *str);

/*
 * exit_shell: Closes down the shell by deallocating the shell state.
 *
 * shell: The current shell state value.
 */
void exit_shell(msh_t *shell);

#endif