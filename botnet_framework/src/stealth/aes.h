#ifndef _AES_H_
#define _AES_H_

#include <stdint.h>

// #define AES256 1
#define AES_BLOCKLEN 16 // Block length in bytes 128 bit
#define AES_KEYLEN 32   // Key length in bytes 256 bit
#define AES_keyExpSize 240

struct AES_ctx
{
  uint8_t RoundKey[AES_keyExpSize];
  uint8_t Iv[AES_BLOCKLEN];
};

void AES_init_ctx(struct AES_ctx* ctx, const uint8_t* key);
void AES_init_ctx_iv(struct AES_ctx* ctx, const uint8_t* key, const uint8_t* iv);
void AES_ctx_set_iv(struct AES_ctx* ctx, const uint8_t* iv);

// buffer size is exactly AES_BLOCKLEN bytes; 
// you need only AES_init_ctx as IV is not used in ECB
// NB: ECB is considered insecure for most uses
void AES_ECB_encrypt(struct AES_ctx* ctx, uint8_t* buf);
void AES_ECB_decrypt(struct AES_ctx* ctx, uint8_t* buf);

// buffer size MUST be mutiple of AES_BLOCKLEN bytes
void AES_CBC_encrypt_buffer(struct AES_ctx* ctx, uint8_t* buf, uint32_t length);
void AES_CBC_decrypt_buffer(struct AES_ctx* ctx, uint8_t* buf, uint32_t length);

#endif //_AES_H_ 
