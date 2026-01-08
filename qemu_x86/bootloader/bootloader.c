/*
 * Stage-2 OTA Bootloader — ROLLBACK Screenshot
 * 32-bit protected mode
 * VGA text mode
 */

#include <stdint.h>

/* =====================================================
 * VGA TEXT MODE
 * ===================================================== */
static volatile uint16_t *vga = (volatile uint16_t *)0xB8000;

static void vga_clear(void)
{
    for (int i = 0; i < 80 * 25; i++)
        vga[i] = 0x0720;
}

static void vga_print(int row, int col, const char *s, uint8_t color)
{
    while (*s) {
        vga[row * 80 + col] = ((uint16_t)color << 8) | *s;
        s++;
        col++;
    }
}

/* =====================================================
 * FIRMWARE PAYLOADS (demo)
 * ===================================================== */
static void firmware_a_main(void)
{
    vga_print(4, 0, "Firmware A running", 0x0A);
    while (1)
        __asm__ volatile ("hlt");
}

/* =====================================================
 * BOOTLOADER ENTRY — ROLLBACK STATE
 * ===================================================== */
void bootloader_main(void)
{
    vga_clear();

    vga_print(0, 0, "OTA BOOTLOADER", 0x0F);
    vga_print(2, 0, "State: ROLLBACK", 0x0C);

    firmware_a_main();
}
