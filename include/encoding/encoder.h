#ifndef ENCODER_H
#define ENCODER_H

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include "watermark/watermark.h"

int encode_watermark(const watermark_t *wm, uint8_t **out, size_t *out_size);
int decode_watermark(const uint8_t *data, size_t size, watermark_t *wm);

#endif