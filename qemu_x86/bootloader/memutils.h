#ifndef MEMUTILS_H
#define MEMUTILS_H

#include <stdint.h>

void *memcpy(void *dest, const void *src, uint32_t n);
void *memset(void *dest, int val, uint32_t n);

#endif
