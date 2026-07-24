#include "utils.h"
#include <stdio.h>
#include <openssl/evp.h>

void sha256(const uint8_t *data, size_t len, uint8_t out[32])
{
EVP_MD_CTX *ctx = EVP_MD_CTX_new();
EVP_DigestInit_ex(ctx, EVP_sha256(), NULL);
EVP_DigestUpdate(ctx, data, len);
unsigned int out_len;
EVP_DigestFinal_ex(ctx, out, &out_len)
EVP_MD_CTX_free(ctx);
}

void double_sha256(const uint8_t *data, size_t len, uint8_t out[32])
{
uint8_t first[32];
sha256(data, len, first);
sha256(first, 32, out);
}

void hex_encode(const uint8_t *in, size_t len, char *out)
{
for(size_t i = 0; i < len; i++)
{
sprintf(out + (i * 2), "%02x", in[i]);
} 
out[len * 2] = '';
}



