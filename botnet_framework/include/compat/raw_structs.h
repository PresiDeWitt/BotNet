#ifndef RAW_STRUCTS_H
#define RAW_STRUCTS_H

#include <stdint.h>

// Definiciones manuales y con nombres únicos para evitar conflictos en macOS.
// Esta es nuestra propia "plantilla" para construir paquetes crudos.

struct my_iphdr {
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

struct my_udphdr {
    uint16_t source;
    uint16_t dest;
    uint16_t len;
    uint16_t check;
};

// La pieza que faltaba: nuestra propia cabecera TCP
struct my_tcphdr {
    uint16_t   source;
    uint16_t   dest;
    uint32_t   seq;
    uint32_t   ack_seq;
    uint16_t   res1:4, doff:5, fin:1, syn:1, rst:1, psh:1, ack:1, urg:1, ece:1, cwr:1;
    uint16_t   window;
    uint16_t   check;
    uint16_t   urg_ptr;
};


#endif // RAW_STRUCTS_H
