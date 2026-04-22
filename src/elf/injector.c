#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <elf.h>
#include "elf/injector.h"


long get_file_size(FILE *f) {
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    rewind(f);
    return size;
}


long align_offset(long offset, int alignment) {
    long remainder = offset % alignment;
    if (remainder == 0) return offset;
    return offset + (alignment - remainder);
}


int append_data_aligned(const char *input_path, const char *output_path,
                        const uint8_t *data, size_t size) {

    FILE *in = fopen(input_path, "rb");
    if (!in) {
        perror("Error opening input");
        return -1;
    }

    FILE *out = fopen(output_path, "wb");
    if (!out) {
        perror("Error opening output");
        fclose(in);
        return -1;
    }


    uint8_t buffer[4096];
    size_t n;

    while ((n = fread(buffer, 1, sizeof(buffer), in)) > 0) {
        if (fwrite(buffer, 1, n, out) != n) {
            perror("Error writing");
            fclose(in);
            fclose(out);
            return -1;
        }
    }


    long offset = ftell(out);


    long aligned = align_offset(offset, 8);


    for (long i = offset; i < aligned; i++) {
        fputc(0x00, out);
    }


    if (fwrite(data, 1, size, out) != size) {
        perror("Error writing data");
        fclose(in);
        fclose(out);
        return -1;
    }

    fclose(in);
    fclose(out);

    return 0;
}

#define ALIGN(x, a) (((x) + (a - 1)) & ~(a - 1))

int inject_section(const char *input,
                   const char *output,
                   const uint8_t *data,
                   size_t data_size) {

    FILE *f = fopen(input, "rb");
    if (!f) return -1;


    fseek(f, 0, SEEK_END);
    size_t file_size = ftell(f);
    rewind(f);

    uint8_t *file = malloc(file_size);
    fread(file, 1, file_size, f);
    fclose(f);

    Elf64_Ehdr *ehdr = (Elf64_Ehdr *)file;
    Elf64_Shdr *shdrs = (Elf64_Shdr *)(file + ehdr->e_shoff);


    Elf64_Shdr shstr = shdrs[ehdr->e_shstrndx];
    char *old_shstrtab = (char *)(file + shstr.sh_offset);


    const char *new_name = ".my_mark";
    size_t new_name_len = strlen(new_name) + 1;

    size_t new_shstr_size = shstr.sh_size + new_name_len;
    char *new_shstrtab = malloc(new_shstr_size);

    memcpy(new_shstrtab, old_shstrtab, shstr.sh_size);
    memcpy(new_shstrtab + shstr.sh_size, new_name, new_name_len);

    size_t name_offset = shstr.sh_size;


    size_t offset = file_size;
    offset = ALIGN(offset, 8);

    size_t data_offset = offset;
    offset += data_size;

    offset = ALIGN(offset, 8);
    size_t shstr_offset = offset;
    offset += new_shstr_size;

    offset = ALIGN(offset, 8);
    size_t new_shoff = offset;


    int new_shnum = ehdr->e_shnum + 1;
    Elf64_Shdr *new_shdrs = malloc(sizeof(Elf64_Shdr) * new_shnum);

    memcpy(new_shdrs, shdrs, sizeof(Elf64_Shdr) * ehdr->e_shnum);


    new_shdrs[ehdr->e_shstrndx].sh_offset = shstr_offset;
    new_shdrs[ehdr->e_shstrndx].sh_size = new_shstr_size;


    Elf64_Shdr *new_sec = &new_shdrs[new_shnum - 1];
    memset(new_sec, 0, sizeof(Elf64_Shdr));

    new_sec->sh_name = name_offset;
    new_sec->sh_type = SHT_PROGBITS;
    new_sec->sh_flags = 0;
    new_sec->sh_offset = data_offset;
    new_sec->sh_size = data_size;
    new_sec->sh_addralign = 1;


    FILE *out = fopen(output, "wb");
    if (!out) return -1;


    fwrite(file, 1, file_size, out);


    size_t pos = file_size;
    while (pos < data_offset) {
        fputc(0, out);
        pos++;
    }


    fwrite(data, 1, data_size, out);


    pos = data_offset + data_size;
    while (pos < shstr_offset) {
        fputc(0, out);
        pos++;
    }


    fwrite(new_shstrtab, 1, new_shstr_size, out);


    pos = shstr_offset + new_shstr_size;
    while (pos < new_shoff) {
        fputc(0, out);
        pos++;
    }


    fwrite(new_shdrs, sizeof(Elf64_Shdr), new_shnum, out);

    fclose(out);


    ehdr->e_shoff = new_shoff;
    ehdr->e_shnum = new_shnum;


    FILE *fix = fopen(output, "r+b");
    fwrite(ehdr, 1, sizeof(Elf64_Ehdr), fix);
    fclose(fix);

    free(file);
    free(new_shstrtab);
    free(new_shdrs);

    return 0;
}