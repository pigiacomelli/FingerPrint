#include <stdlib.h>
#include <string.h>
#include "encoding/encoder.h"

int encode_watermark(const watermark_t *wm, uint8_t **out, size_t *out_size) {
    if (!wm || !out || !out_size) return -1;

    *out_size = sizeof(watermark_t);
    *out = malloc(*out_size);
    if (!*out) return -1;

    memcpy(*out, wm, *out_size);
    return 0;
}

int decode_watermark(const uint8_t *data, size_t size, watermark_t *wm) {
    if (!data || !wm || size != sizeof(watermark_t)) return -1;

    memcpy(wm, data, sizeof(watermark_t));
    return 0;
}