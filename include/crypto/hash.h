#ifndef HASH_H
#define HASH_H

#include <stddef.h>
#include <stdint.h>

void generate_hash(const uint8_t *data, size_t len, uint8_t hash[32]);

#endif