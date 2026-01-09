#include <stdint.h>
void *memcpy(void *dest, const void *src, uint32_t n)
{
    uint8_t *d = (uint8_t *)dest;
    const uint8_t *s = (const uint8_t *)src;

    while (n--)
        *d++ = *s++;

    return dest;
}

void *memset(void *dest, int val, uint32_t n)
{
    uint8_t *d = (uint8_t *)dest;

    while (n--)
        *d++ = (uint8_t)val;

    return dest;
}
