#include "../../include/botnet.h"
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

// The actual signal handler function
void handle_signal(int signo) {
    if (signo == SIGINT) {
        printf("\n[*] Received SIGINT. Shutting down gracefully...\n");
        bot_stop();
        exit(0);
    }
}

// Sets up the signal handlers
void setup_signal_handler() {
    if (signal(SIGINT, handle_signal) == SIG_ERR) {
        perror("signal");
        printf("[-] Could not set up signal handler.\n");
    }
}
