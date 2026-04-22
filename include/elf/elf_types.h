#ifndef ELF_TYPES_H
#define ELF_TYPES_H

#include <elf.h>

typedef struct {
    Elf64_Ehdr ehdr;
    Elf64_Shdr *shdrs;
    char *shstrtab;
} elf_t;

#endif