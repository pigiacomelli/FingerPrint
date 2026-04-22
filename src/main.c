#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "watermark/watermark.h"
#include "encoding/encoder.h"
#include "crypto/hash.h"
#include "elf/injector.h"
#include "elf/extractor.h"

void print_usage(const char *prog) {
    printf("Usage: %s <command> [args]\n", prog);
    printf("Commands:\n");
    printf("  inject <input_elf> <output_elf> <client_id>  Inject a watermark into an ELF file\n");
    printf("  extract <elf_file>                           Extract a watermark from an ELF file\n");
}

int handle_inject(int argc, char **argv) {
    if (argc < 5) {
        printf("Usage: inject <input_elf> <output_elf> <client_id>\n");
        return 1;
    }

    const char *input = argv[2];
    const char *output = argv[3];
    const char *client_id = argv[4];

    watermark_t wm = {0};
    strncpy(wm.client_id, client_id, CLIENT_ID_SIZE - 1);

    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(wm.timestamp, TIMESTAMP_SIZE, "%Y-%m-%d %H:%M:%S", t);

    generate_hash((uint8_t*)&wm, sizeof(wm) - 32, wm.hash);

    uint8_t *encoded = NULL;
    size_t size = 0;
    if (encode_watermark(&wm, &encoded, &size) != 0) {
        fprintf(stderr, "Error: Failed to encode watermark\n");
        return 1;
    }

    if (inject_section(input, output, encoded, size) != 0) {
        fprintf(stderr, "Error: Failed to inject watermark into %s\n", input);
        free(encoded);
        return 1;
    }

    printf("Successfully injected watermark into %s (Output: %s)\n", input, output);
    printf("Client ID: %s\n", wm.client_id);
    printf("Timestamp: %s\n", wm.timestamp);

    free(encoded);
    return 0;
}

int handle_extract(int argc, char **argv) {
    if (argc < 3) {
        printf("Usage: extract <elf_file>\n");
        return 1;
    }

    const char *path = argv[2];
    watermark_t wm;

    if (extract_watermark(path, &wm) != 0) {
        fprintf(stderr, "Error: Could not extract watermark from %s\n", path);
        return 1;
    }

    printf("=== Watermark Extracted from %s ===\n", path);
    printf("Client ID: %s\n", wm.client_id);
    printf("Timestamp: %s\n", wm.timestamp);
    printf("Hash: ");
    for (int i = 0; i < 32; i++) {
        printf("%02x", wm.hash[i]);
    }
    printf("\n");

    return 0;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        print_usage(argv[0]);
        return 1;
    }

    const char *cmd = argv[1];

    if (strcmp(cmd, "inject") == 0) {
        return handle_inject(argc, argv);
    } else if (strcmp(cmd, "extract") == 0) {
        return handle_extract(argc, argv);
    } else {
        fprintf(stderr, "Error: Unknown command '%s'\n", cmd);
        print_usage(argv[0]);
        return 1;
    }

    return 0;
}
