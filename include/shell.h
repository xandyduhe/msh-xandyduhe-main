#ifndef _SHELL_H_
#define _SHELL_H_

#include <stdbool.h>
 
// Represents the state of the shell 
typedef struct msh {
    /** TODO: IMPLEMENT **/ 
    int max_jobs;       // Maximum number of jobs allowed
    int max_line;       // Maximum number of characters per line
    int max_history;    // Maximum number of commands in history
    // Other fields to store shell state as needed
}msh_t;

// Global shell state
extern msh_t *shell;

/*
 * alloc_shell: Allocates and initializes the state of the shell
 *
 * max_jobs: The maximum number of jobs that can be in existence at any point in time.
 * max_line: The maximum number of characters that can be entered for any specific command line.
 * max_history: The maximum number of saved history commands for the shell.
 *
 * Returns: a pointer to an allocated and initialized msh_t structure
 */
msh_t *alloc_shell(int max_jobs, int max_line, int max_history);

/**
 * parse_tok: Continuously retrieves separate commands from the provided command line until all commands are parsed
 *
 * line:  the command line to parse, which may include multiple commands. If line is NULL, parse_tok continues parsing the previous command line.
 * job_type: Specifies whether the parsed command is a background (0) or foreground job (1). If no job is returned, assigns -1.
 *
 * Returns: NULL if no more commands can be parsed; otherwise, returns a parsed command from the command line.
 */
char *parse_tok(char *line, int *job_type);

/**
 * separate_args: Separates the arguments of a command and places them in an allocated array returned by this function
 *
 * line: the command line to separate (assumes only a single command with zero or more arguments).
 * argc: Stores the number of arguments produced at the memory location of the argc pointer.
 * is_builtin: true if the command is a built-in command; otherwise false.
 *
 * Returns: NULL if line contains no arguments; otherwise, a newly allocated array of strings representing the arguments of the command (similar to argv). The array includes a NULL in its last location.
 * Note: The user is responsible for freeing the memory returned by this function.
 */
char **separate_args(char *line, int *argc, bool *is_builtin);

/*
 * evaluate - Executes the provided command line string
 *
 * shell - the current shell state value
 * line - the command line string to evaluate
 *
 * Returns: non-zero if the command executed wants the shell program to close. Otherwise, 0.
 */
int evaluate(msh_t *shell, char *line);

/*
 * exit_shell - Closes down the shell by deallocating the shell state.
 *
 * shell - the current shell state value
 */
void exit_shell(msh_t *shell);

#endif // SHELL_H