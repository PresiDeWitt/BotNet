#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdint.h>

#define MAX_MESSAGE_SIZE 4096

// Defines the structure for messages exchanged within the botnet
typedef struct {
    uint32_t message_id;
    uint8_t  message_type;
    uint32_t timestamp;
    uint16_t data_len;
    uint8_t  data[MAX_MESSAGE_SIZE]; // Fixed-size buffer for simplicity
} botnet_message_t;

// Function prototypes for message handling
void encrypt_message(botnet_message_t *msg, char *key);
void decrypt_message(botnet_message_t *msg, char *key);
int send_message(int sock, botnet_message_t *msg, char *encryption_key);
int receive_message(int sock, botnet_message_t *msg, char *encryption_key);

#endif // PROTOCOL_H
