#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "watermark/watermark.h"
#include "encoding/encoder.h"
#include "crypto/hash.h"
#include "elf/injector.h"

int main() {
    printf("=== INJECTOR TEST (.my_mark) ===\n\n");


    watermark_t wm = {0};

    strncpy(wm.client_id, "client123", CLIENT_ID_SIZE - 1);
    strncpy(wm.timestamp, "2026-04-21", TIMESTAMP_SIZE - 1);


    generate_hash((uint8_t*)&wm, sizeof(wm) - 32, wm.hash);

    printf("[+] Watermark created\n");


    uint8_t *encoded = NULL;
    size_t size = 0;

    if (encode_watermark(&wm, &encoded, &size) != 0) {
        printf("[FAIL] Encode failed\n");
        return 1;
    }

    printf("[+] Encode OK (%zu bytes)\n", size);


    const char *input  = "samples/target_app";
    const char *output = "build/target_app_marked";

    if (inject_section(input, output, encoded, size) != 0) {
        printf("[FAIL] Injector failed\n");
        free(encoded);
        return 1;
    }

    printf("[+] Injection completed\n");

    free(encoded);


    printf("\n=== VALIDATION ===\n");
    printf("1. Check if the binary executes:\n");
    printf("   ./build/target_app_marked\n\n");

    printf("2. Check if the section exists:\n");
    printf("   readelf -S build/target_app_marked | grep my_mark\n\n");

    printf("If everything is correct:\n");
    printf("✔ binary executes\n");
    printf("✔ .my_mark appears in readelf\n");

    return 0;
}