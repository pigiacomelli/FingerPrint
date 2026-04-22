#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "encoding/encoder.h"
#include "watermark/watermark.h"
#include "crypto/hash.h"

int main() {
    watermark_t wm;
    memset(&wm, 0, sizeof(watermark_t));

    strncpy(wm.client_id, "user-example-001", CLIENT_ID_SIZE - 1);
    strncpy(wm.timestamp, "2026-04-21 11:00:00", TIMESTAMP_SIZE - 1);


    const char *dummy_data = "data to generate hash";
    generate_hash((uint8_t *)dummy_data, strlen(dummy_data), wm.hash);

    printf("--- Encoding Test ---\n");
    printf("Client ID: %s\n", wm.client_id);
    printf("Timestamp: %s\n", wm.timestamp);
    printf("Hash: ");
    for (int i = 0; i < 8; i++) printf("%02x", wm.hash[i]);
    printf("... (summarized)\n");

    uint8_t *encoded_data = NULL;
    size_t encoded_size = 0;

    if (encode_watermark(&wm, &encoded_data, &encoded_size) != 0) {
        fprintf(stderr, "Error encoding watermark\n");
        return 1;
    }

    printf("Watermark encoded successfully (%zu bytes)\n", encoded_size);

    watermark_t decoded_wm;
    if (decode_watermark(encoded_data, encoded_size, &decoded_wm) != 0) {
        fprintf(stderr, "Error decoding watermark\n");
        free(encoded_data);
        return 1;
    }

    if (memcmp(&wm, &decoded_wm, sizeof(watermark_t)) == 0) {
        printf("[OK] Integrity test passed: Decoded data is identical to original\n");
    } else {
        printf("[FAIL] Integrity test failed: Decoded data is different\n");
    }

    free(encoded_data);
    return 0;
}