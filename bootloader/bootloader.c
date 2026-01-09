#include <stdint.h>
static volatile uint16_t *vga = (volatile uint16_t *)0xB8000;
static int cursor = 0;

static void vga_clear(void)
{
    for (int i = 0; i < 80 * 25; i++)
        vga[i] = 0x0720;
    cursor = 0;
}

static void vga_putc(char c, uint8_t color)
{
    vga[cursor++] = ((uint16_t)color << 8) | c;
}

static void vga_puts(const char *s, uint8_t color)
{
    while (*s)
        vga_putc(*s++, color);
    vga_putc('\n', color);
}

#define OTA_MAGIC 0xDEADBEEF
#define MAX_TEST_ATTEMPTS 3
#define OTA_META_LBA_PRIMARY  10
#define OTA_META_LBA_BACKUP   11

typedef enum {
    OTA_STATE_IDLE = 0,
    OTA_STATE_UPDATE_PENDING,
    OTA_STATE_TESTING,
    OTA_STATE_CONFIRMED,
    OTA_STATE_ROLLBACK
} ota_state_t;

typedef enum {
    SLOT_A = 0,
    SLOT_B = 1
} slot_t;

typedef struct {
    uint32_t magic;
    uint32_t version;
    ota_state_t state;
    slot_t active_slot;
    slot_t pending_slot;
    uint32_t boot_attempts;
    uint32_t crc;
} ota_metadata_t;
#define ATA_DATA       0x1F0
#define ATA_SECTOR_CNT 0x1F2
#define ATA_LBA_LO     0x1F3
#define ATA_LBA_MID    0x1F4
#define ATA_LBA_HI     0x1F5
#define ATA_DRIVE      0x1F6
#define ATA_COMMAND    0x1F7
#define ATA_STATUS     0x1F7

#define ATA_CMD_READ   0x20
#define ATA_CMD_WRITE  0x30

static inline void outb(uint16_t p, uint8_t v)
{
    __asm__ volatile ("outb %0,%1"::"a"(v),"Nd"(p));
}

static inline uint16_t inw(uint16_t p)
{
    uint16_t r;
    __asm__ volatile ("inw %1,%0":"=a"(r):"Nd"(p));
    return r;
}

static inline uint8_t inb(uint16_t p)
{
    uint8_t r;
    __asm__ volatile ("inb %1,%0":"=a"(r):"Nd"(p));
    return r;
}

static inline void outw(uint16_t p, uint16_t v)
{
    __asm__ volatile ("outw %0,%1"::"a"(v),"Nd"(p));
}

static void ata_wait(void)
{
    while (inb(ATA_STATUS) & 0x80);
}

static void disk_read_sector(uint32_t lba, void *buf)
{
    ata_wait();
    outb(ATA_DRIVE, 0xE0 | ((lba >> 24) & 0x0F));
    outb(ATA_SECTOR_CNT, 1);
    outb(ATA_LBA_LO, lba);
    outb(ATA_LBA_MID, lba >> 8);
    outb(ATA_LBA_HI, lba >> 16);
    outb(ATA_COMMAND, ATA_CMD_READ);
    ata_wait();

    uint16_t *p = buf;
    for (int i = 0; i < 256; i++)
        p[i] = inw(ATA_DATA);
}

static void disk_write_sector(uint32_t lba, const void *buf)
{
    ata_wait();
    outb(ATA_DRIVE, 0xE0 | ((lba >> 24) & 0x0F));
    outb(ATA_SECTOR_CNT, 1);
    outb(ATA_LBA_LO, lba);
    outb(ATA_LBA_MID, lba >> 8);
    outb(ATA_LBA_HI, lba >> 16);
    outb(ATA_COMMAND, ATA_CMD_WRITE);
    ata_wait();

    const uint16_t *p = buf;
    for (int i = 0; i < 256; i++)
        outw(ATA_DATA, p[i]);
}

static uint32_t crc32(const void *d, uint32_t n)
{
    const uint8_t *p = d;
    uint32_t c = 0xFFFFFFFF;

    for (uint32_t i = 0; i < n; i++) {
        c ^= p[i];
        for (int j = 0; j < 8; j++)
            c = (c >> 1) ^ (0xEDB88320 & (-(c & 1)));
    }
    return ~c;
}

static ota_metadata_t meta;

static int meta_valid(const ota_metadata_t *m)
{
    if (m->magic != OTA_MAGIC) return 0;
    return crc32(m, sizeof(*m) - sizeof(uint32_t)) == m->crc;
}

static void meta_write(uint32_t lba)
{
    meta.crc = crc32(&meta, sizeof(meta) - sizeof(uint32_t));
    disk_write_sector(lba, &meta);
}

static void ota_metadata_load(void)
{
    ota_metadata_t a, b;
    disk_read_sector(OTA_META_LBA_PRIMARY, &a);
    disk_read_sector(OTA_META_LBA_BACKUP, &b);

    if (meta_valid(&a)) meta = a;
    else if (meta_valid(&b)) meta = b;
    else {
        meta.magic = OTA_MAGIC;
        meta.version = 1;
        meta.state = OTA_STATE_IDLE;
        meta.active_slot = SLOT_A;
        meta.pending_slot = SLOT_B;
        meta.boot_attempts = 0;
        meta_write(OTA_META_LBA_PRIMARY);
        meta_write(OTA_META_LBA_BACKUP);
    }
}

static void ota_metadata_save(void)
{
    meta.version++;
    meta_write(OTA_META_LBA_PRIMARY);
    meta_write(OTA_META_LBA_BACKUP);
}


void ota_confirm(void)
{
    vga_puts("Firmware CONFIRMED", 0x02);
    meta.state = OTA_STATE_CONFIRMED;
    meta.boot_attempts = 0;
    ota_metadata_save();
}

static void firmware_a_main(void)
{
    vga_puts("Firmware A running", 0x0A);
    while (1) __asm__ volatile ("hlt");
}

static void firmware_b_main(void)
{
    vga_puts("Firmware B running (TEST)", 0x0B);
    ota_confirm();
    while (1) __asm__ volatile ("hlt");
}

static void jump_to_firmware(slot_t s)
{
    if (s == SLOT_A) firmware_a_main();
    else firmware_b_main();
}


void bootloader_main(void)
{
    vga_clear();
    vga_puts("OTA BOOTLOADER", 0x0F);

    ota_metadata_load();

    switch (meta.state) {

    case OTA_STATE_UPDATE_PENDING:
        meta.state = OTA_STATE_TESTING;
        meta.boot_attempts = 0;
        ota_metadata_save();


    case OTA_STATE_TESTING:
        meta.boot_attempts++;
        ota_metadata_save();

        vga_puts("State: TESTING", 0x0E);
        vga_puts("Firmware B running (TEST)", 0x0B);
        jump_to_firmware(SLOT_B);
        break;

    default:
        jump_to_firmware(meta.active_slot);
        break;
    }

    while (1) __asm__ volatile ("hlt");
}
