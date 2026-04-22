#ifndef INJECTOR_H
#define INJECTOR_H

#include <stddef.h>
#include <stdint.h>

int append_data_aligned(const char *input_path, const char *output_path,
                        const uint8_t *data, size_t size);

int inject_section(const char *input,
                   const char *output,
                   const uint8_t *data,
                   size_t data_size);

#endif