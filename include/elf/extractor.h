#ifndef EXTRACTOR_H
#define EXTRACTOR_H

#include "../watermark/watermark.h"

int extract_watermark(const char *path, watermark_t *out);

#endif