#ifndef PARSER_H
#define PARSER_H

#include "elf_types.h"

int load_elf(const char *path, elf_t *elf);
void free_elf(elf_t *elf);
void print_sections(const elf_t *elf);
int find_section(const elf_t *elf, const char *name);
#endif