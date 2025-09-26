#include "../../include/botnet.h"
#include "../stealth/aes.h"
#include <string.h>

// Generates a 32-byte key from the config string for AES-256
static void get_aes_key(uint8_t* key_buf) {
    const char* key_str = AES_KEY;
    // For simplicity, we use the first 32 bytes. In a real scenario, a key derivation
    // function like PBKDF2 or HKDF would be used.
    strncpy((char*)key_buf, key_str, 32);
}

// Encrypts a buffer using AES-256-CBC
int aes_encrypt(const uint8_t* in, int in_len, uint8_t* out) {
    struct AES_ctx ctx;
    uint8_t key[32];
    uint8_t iv[16];

    // 1. Generate a random IV
    // For a real bot, a better source of randomness would be used.
    for(int i=0; i<16; ++i) iv[i] = rand() % 256;

    // 2. Initialize context
    get_aes_key(key);
    AES_init_ctx_iv(&ctx, key, iv);

    // 3. Calculate padding (PKCS#7)
    int padding = AES_BLOCKLEN - (in_len % AES_BLOCKLEN);
    int padded_len = in_len + padding;

    // 4. Copy input and apply padding
    memcpy(out + AES_BLOCKLEN, in, in_len);
    for (int i = 0; i < padding; i++) {
        out[AES_BLOCKLEN + in_len + i] = padding;
    }

    // 5. Encrypt the padded buffer
    AES_CBC_encrypt_buffer(&ctx, out + AES_BLOCKLEN, padded_len);

    // 6. Prepend the IV to the ciphertext
    memcpy(out, iv, AES_BLOCKLEN);

    return AES_BLOCKLEN + padded_len;
}

// Decrypts a buffer using AES-256-CBC
int aes_decrypt(uint8_t* buf, int buf_len) {
    if (buf_len <= AES_BLOCKLEN || (buf_len % AES_BLOCKLEN) != 0) {
        return -1; // Invalid length
    }

    struct AES_ctx ctx;
    uint8_t key[32];
    uint8_t iv[16];

    // 1. Extract the IV from the beginning of the buffer
    memcpy(iv, buf, AES_BLOCKLEN);

    // 2. Initialize context
    get_aes_key(key);
    AES_init_ctx_iv(&ctx, key, iv);

    // 3. Decrypt the buffer (ciphertext is after the IV)
    uint8_t* ciphertext = buf + AES_BLOCKLEN;
    int ciphertext_len = buf_len - AES_BLOCKLEN;
    AES_CBC_decrypt_buffer(&ctx, ciphertext, ciphertext_len);

    // 4. Validate and remove PKCS#7 padding
    int padding = ciphertext[ciphertext_len - 1];
    if (padding > AES_BLOCKLEN || padding == 0) {
        return -1; // Invalid padding
    }
    for (int i = 0; i < padding; i++) {
        if (ciphertext[ciphertext_len - 1 - i] != padding) {
            return -1; // Invalid padding byte
        }
    }

    // 5. Return the original length
    return ciphertext_len - padding;
}
