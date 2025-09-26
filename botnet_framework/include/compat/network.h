#ifndef NETWORK_COMPAT_H
#define NETWORK_COMPAT_H

// Este archivo es la solución definitiva para los errores de tipo incompleto en macOS.

#include <stdint.h>

#if defined(__APPLE__) && defined(__MACH__)

// macOS no expone estas estructuras fácilmente. Las definimos manualmente.
// Esta es la única fuente de verdad para estas estructuras en nuestro proyecto.

#ifndef __IP_HDR__
#define __IP_HDR__
struct iphdr {
    uint8_t    ihl:4, version:4;
    uint8_t    tos;
    uint16_t   tot_len;
    uint16_t   id;
    uint16_t   frag_off;
    uint8_t    ttl;
    uint8_t    protocol;
    uint16_t   check;
    uint32_t   saddr;
    uint32_t   daddr;
};
#endif

#ifndef __TCP_HDR__
#define __TCP_HDR__
struct tcphdr {
    uint16_t   source;
    uint16_t   dest;
    uint32_t   seq;
    uint32_t   ack_seq;
    uint16_t   res1:4, doff:4, fin:1, syn:1, rst:1, psh:1, ack:1, urg:1, ece:1, cwr:1;
    uint16_t   window;
    uint16_t   check;
    uint16_t   urg_ptr;
};
#endif

#ifndef __UDP_HDR__
#define __UDP_HDR__
struct udphdr {
    uint16_t source;
    uint16_t dest;
    uint16_t len;
    uint16_t check;
};
#endif

#else

// Para Linux y otros sistemas estándar, simplemente incluimos las cabeceras del sistema.
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>

#endif

#endif // NETWORK_COMPAT_H
