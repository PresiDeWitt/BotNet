#ifndef NETWORK_UTILS_H
#define NETWORK_UTILS_H

#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include "../structures.h"

// Utilidades de red
uint32_t get_local_ip();
uint32_t get_gateway_ip();
int check_connectivity();
int get_network_interfaces();

// Socket utilities
int create_socket(int type, int protocol);
int set_socket_options(int sock, int options);
int bind_socket(int sock, uint32_t ip, uint16_t port);

// Protocolos
unsigned short calculate_checksum(void *data, int len);
void build_ip_header(struct iphdr *ip, uint32_t src, uint32_t dst, int protocol);
void build_tcp_header(struct tcphdr *tcp, uint16_t src_port, uint16_t dst_port);

#endif