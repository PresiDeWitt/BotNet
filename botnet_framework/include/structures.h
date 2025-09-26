#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <stdint.h>

// Defines a target for an attack or infection attempt
struct target {
    uint32_t addr; // Target IP address in network byte order
    uint16_t port; // Target port in network byte order
};

// Defines the parameters for a command-and-control issued attack
struct attack {
    uint8_t type;         // The attack vector to use (e.g., 1 for TCP, 2 for UDP)
    int threads;      // The number of concurrent threads for the attack
    struct target target; // The target to attack
};

// Defines the status report sent from a bot to the C2
struct bot_status_report {
    char arch[16]; // e.g., "x86_64", "armv7l"
    long uptime;   // Bot uptime in seconds
    int infections; // Number of successful infections initiated by this bot
};

// ¡LA NUEVA ESTRUCTURA!
// Holds configuration parsed from a file.
typedef struct {
    int c2_port;
    int p2p_port;
    int max_threads;
} botnet_config_t;


#endif // STRUCTURES_H
