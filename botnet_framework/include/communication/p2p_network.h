#ifndef P2P_NETWORK_H
#define P2P_NETWORK_H

// Initializes the P2P module and starts the gossip thread
void p2p_init();

// Adds a new peer to our list of known peers
void p2p_add_peer(const char *peer_addr);

// Broadcasts a command received from the C2 to the P2P network
void p2p_broadcast_command(const char *command);

#endif // P2P_NETWORK_H
