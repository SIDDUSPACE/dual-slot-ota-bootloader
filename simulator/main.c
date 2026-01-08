#include <stdio.h>
#include "bootloader.h"
#include "ota_updater.h"

/*
 * This simulates a real device:
 * - boot
 * - optional OTA
 * - reboot
 */

int main(void)
{
    printf("\n=== DEVICE POWER ON ===\n");
    bootloader_main();

    printf("\n=== TRIGGER OTA UPDATE ===\n");
    ota_perform_update(2);   // try version 2

    printf("\n=== DEVICE REBOOT ===\n");
    bootloader_main();

    printf("\n=== DEVICE REBOOT AGAIN ===\n");
    bootloader_main();

    return 0;
}
