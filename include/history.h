#ifndef _HISTORY_H_
#define _HISTORY_H_

extern const char *HISTORY_FILE_PATH;

/*
 * Represents the state of the history of the shell.
 */
typedef struct history {
    char **lines;      // Array of command lines
    int max_history;   // Maximum number of history lines
    int next;          // Next available slot in the history
} history_t;

/*
 * alloc_history: Allocates and initializes a history_t structure.
 * 
 * max_history: Maximum number of history lines.
 * 
 * Returns: A pointer to the allocated history_t structure or NULL on failure.
 */
history_t *alloc_history(int max_history);

/*
 * add_line_history: Adds a command line to the history.
 * 
 * history: Pointer to the history structure.
 * cmd_line: The command line to add.
 */
void add_line_history(history_t *history, const char *cmd_line);

/*
 * print_history: Prints the current command history.
 * 
 * history: Pointer to the history structure.
 */
void print_history(history_t *history);

/*
 * find_line_history: Retrieves a specific line from the history.
 * 
 * history: Pointer to the history structure.
 * index: 1-based index of the history line to retrieve.
 * 
 * Returns: The command line at the given index or NULL if the index is invalid.
 */
char *find_line_history(history_t *history, int index);

/*
 * free_history: Frees the allocated history structure and writes history to the file.
 * 
 * history: Pointer to the history structure.
 */
void free_history(history_t *history);

#endif // _HISTORY_H_
