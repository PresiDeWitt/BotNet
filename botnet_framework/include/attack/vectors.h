#ifndef VECTORS_H
#define VECTORS_H

#include "../structures.h"

// Worker functions for different attack vectors.
// These are started in threads by the attack_manager.

void *tcp_flood_worker(void *args);
void *udp_flood_worker(void *args);
void *http_flood_worker(void *args);
void *slowloris_worker(void *args);
void *dns_amp_worker(void *args);
void *ntp_amp_worker(void *args);
void *ssdp_amp_worker(void *args);

#endif // VECTORS_H
