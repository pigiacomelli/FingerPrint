#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <elf.h>

#include "elf/extractor.h"
#include "elf/parser.h"
#include "encoding/encoder.h"

int extract_watermark(const char *path, watermark_t *out) {
    elf_t elf;
    if (load_elf(path, &elf) != 0) {
        return -1;
    }

    int idx = find_section(&elf, ".my_mark");
    if (idx < 0) {
        free_elf(&elf);
        return -1;
    }

    Elf64_Shdr *shdr = &elf.shdrs[idx];
    
    FILE *f = fopen(path, "rb");
    if (!f) {
        free_elf(&elf);
        return -1;
    }

    uint8_t *encoded_data = malloc(shdr->sh_size);
    if (!encoded_data) {
        fclose(f);
        free_elf(&elf);
        return -1;
    }

    fseek(f, shdr->sh_offset, SEEK_SET);
    if (fread(encoded_data, 1, shdr->sh_size, f) != shdr->sh_size) {
        free(encoded_data);
        fclose(f);
        free_elf(&elf);
        return -1;
    }

    fclose(f);

    if (decode_watermark(encoded_data, shdr->sh_size, out) != 0) {
        free(encoded_data);
        free_elf(&elf);
        return -1;
    }

    free(encoded_data);
    free_elf(&elf);
    return 0;
}
