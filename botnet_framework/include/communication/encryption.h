#ifndef ENCRYPTION_H
#define ENCRYPTION_H

#include <stdint.h>

// Encrypts a buffer using AES-256-CBC. 
// Returns the new length of the buffer after padding and encryption.
// The output buffer must have enough space.
int aes_encrypt(const uint8_t* in, int in_len, uint8_t* out);

// Decrypts a buffer using AES-256-CBC.
// Returns the new length of the buffer after removing padding.
int aes_decrypt(uint8_t* buf, int buf_len);

#endif // ENCRYPTION_H
