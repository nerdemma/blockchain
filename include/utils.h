#ifndef UTILS_H
#define UTILS_H
#include <stddef.h>
#include <stdint.h>

void sha256(const uint8_t *data, size_t len, uint8_t out[32]);

void double_sha256(const uint8_t, *data size_t len, uint8_t uint8_t[32]);
void hex_code(const uint8_ *in, size_t len, char *out);
#endif

