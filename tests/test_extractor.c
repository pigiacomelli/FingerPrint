#include <stdio.h>
#include "../src/elf/extractor.h"

int main() {
    watermark_t wm;

    if (extract_watermark("build/target_app_marked", &wm) != 0) {
        printf("[FAIL] Could not extract watermark\n");
        return 1;
    }

    printf("=== WATERMARK EXTRACTED ===\n\n");

    printf("Client ID: %s\n", wm.client_id);
    printf("Timestamp: %s\n", wm.timestamp);

    printf("Hash: ");
    for (int i = 0; i < 32; i++) {
        printf("%02x ", wm.hash[i]);
    }
    printf("\n");

    return 0;
}