#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "elf/parser.h"

int load_elf(const char *path, elf_t *elf) {
    FILE *f = fopen(path, "rb");
    if (!f) return -1;


    if (fread(&elf->ehdr, 1, sizeof(Elf64_Ehdr), f) != sizeof(Elf64_Ehdr)) {
        fclose(f);
        return -1;
    }


    if (memcmp(elf->ehdr.e_ident, ELFMAG, SELFMAG) != 0) {
        fclose(f);
        return -1;
    }


    elf->shdrs = malloc(elf->ehdr.e_shentsize * elf->ehdr.e_shnum);
    if (!elf->shdrs) {
        fclose(f);
        return -1;
    }


    if (fseek(f, elf->ehdr.e_shoff, SEEK_SET) != 0) {
        free(elf->shdrs);
        fclose(f);
        return -1;
    }


    if (fread(elf->shdrs, elf->ehdr.e_shentsize, elf->ehdr.e_shnum, f) != elf->ehdr.e_shnum) {
        free(elf->shdrs);
        fclose(f);
        return -1;
    }


    Elf64_Shdr shstr = elf->shdrs[elf->ehdr.e_shstrndx];

    elf->shstrtab = malloc(shstr.sh_size);
    if (!elf->shstrtab) {
        free(elf->shdrs);
        fclose(f);
        return -1;
    }

    if (fseek(f, shstr.sh_offset, SEEK_SET) != 0) {
        free(elf->shdrs);
        free(elf->shstrtab);
        fclose(f);
        return -1;
    }

    if (fread(elf->shstrtab, 1, shstr.sh_size, f) != shstr.sh_size) {
        free(elf->shdrs);
        free(elf->shstrtab);
        fclose(f);
        return -1;
    }

    fclose(f);
    return 0;
}

void free_elf(elf_t *elf) {
    if (elf->shdrs) free(elf->shdrs);
    if (elf->shstrtab) free(elf->shstrtab);
}

void print_sections(const elf_t *elf) {
    for (int i = 0; i < elf->ehdr.e_shnum; i++) {
        const char *name = &elf->shstrtab[elf->shdrs[i].sh_name];
        printf("[%2d] %s\n", i, name);
    }
}

int find_section(const elf_t *elf, const char *name) {
    for (int i = 0; i < elf->ehdr.e_shnum; i++) {
        const char *sec_name = &elf->shstrtab[elf->shdrs[i].sh_name];

        if (strcmp(sec_name, name) == 0) {
            return i;
        }
    }
    return -1;
}