// qemu_x86/bootloader/start.c
void bootloader_main(void);

void _start(void)
{
    bootloader_main();

    for (;;)
        __asm__ volatile ("hlt");
}
