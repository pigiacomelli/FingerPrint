#include <stdio.h>
#include "elf/parser.h"

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: %s <binary>\n", argv[0]);
        return 1;
    }

    elf_t elf;

    if (load_elf(argv[1], &elf) != 0) {
        printf("Error loading ELF\n");
        return 1;
    }

    printf("ELF loaded successfully\n\n");

    print_sections(&elf);

    int idx = find_section(&elf, ".text");

    if (idx >= 0) {
        printf("\n[OK] .text found at index: %d\n", idx);
    } else {
        printf("\n[FAIL] .text not found\n");
    }

    free_elf(&elf);
    return 0;
}