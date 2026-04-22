#ifndef WATERMARK_H
#define WATERMARK_H

#include <stdint.h>
#include <stdlib.h>

#define CLIENT_ID_SIZE 32
#define TIMESTAMP_SIZE 32
#define HASH_SIZE 32

typedef struct {
    char client_id[CLIENT_ID_SIZE];
    char timestamp[TIMESTAMP_SIZE];
    uint8_t hash[HASH_SIZE];
} watermark_t;

#endif