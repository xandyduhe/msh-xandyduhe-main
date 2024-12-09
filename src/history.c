#include "history.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char *HISTORY_FILE_PATH = "../data/.msh_history";

/*
 * alloc_history: Allocates and initializes a history_t structure.
 * Loads prior history from the file into the lines array.
 *
 * max_history: Maximum number of history lines.
 *
 * Returns: A pointer to the allocated history_t structure or NULL on failure.
 */
history_t *alloc_history(int max_history) {
    history_t *history = malloc(sizeof(history_t));
    if (!history) {
        perror("malloc");
        return NULL;
    }

    history->lines = calloc(max_history, sizeof(char *));
    if (!history->lines) {
        perror("calloc");
        free(history);
        return NULL;
    }

    history->max_history = max_history;
    history->next = 0;

    // Open the history file to load prior history
    FILE *file = fopen(HISTORY_FILE_PATH, "r");
    if (file) {
        char buffer[1024];
        while (fgets(buffer, sizeof(buffer), file)) {
            size_t len = strlen(buffer);
            if (len > 0 && buffer[len - 1] == '\n') {
                buffer[len - 1] = '\0'; // Remove trailing newline
            }

            // Add the line to the history
            if (history->next < max_history) {
                history->lines[history->next] = strdup(buffer);
                history->next++;
            }
        }
        fclose(file);
    }

    return history;
}

/*
 * add_line_history: Adds a command line to the history.
 * 
 * history: Pointer to the history structure.
 * cmd_line: The command line to add.
 */
void add_line_history(history_t *history, const char *cmd_line) {
    if (!cmd_line || strlen(cmd_line) == 0 || strcmp(cmd_line, "exit") == 0) {
        return; // Do not add empty or "exit" commands
    }

    // If the history is full, remove the oldest entry
    if (history->next == history->max_history) {
        free(history->lines[0]);
        for (int i = 1; i < history->max_history; i++) {
            history->lines[i - 1] = history->lines[i];
        }
        history->next--;
    }

    // Add the new command to the history
    history->lines[history->next] = strdup(cmd_line);
    history->next++;
}

/*
 * print_history: Prints the current command history.
 * 
 * history: Pointer to the history structure.
 */
void print_history(history_t *history) {
    for (int i = 1; i <= history->next; i++) {
        printf("%5d\t%s\n", i, history->lines[i - 1]);
    }
}

/*
 * find_line_history: Retrieves a specific line from the history.
 * 
 * history: Pointer to the history structure.
 * index: 1-based index of the history line to retrieve.
 * 
 * Returns: The command line at the given index or NULL if the index is invalid.
 */
char *find_line_history(history_t *history, int index) {
    if (index < 1 || index > history->next) {
        return NULL;
    }
    return history->lines[index - 1];
}

/*
 * free_history: Frees the allocated history structure and writes history to the file.
 * 
 * history: Pointer to the history structure.
 */
void free_history(history_t *history) {
    // Write history to the file before freeing
    FILE *file = fopen(HISTORY_FILE_PATH, "w");
    if (file) {
        for (int i = 0; i < history->next; i++) {
            fprintf(file, "%s\n", history->lines[i]);
        }
        fclose(file);
    }

    // Free the lines and the history structure
    for (int i = 0; i < history->next; i++) {
        free(history->lines[i]);
    }
    free(history->lines);
    free(history);
}
