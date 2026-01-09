#include <stdio.h>
#include "ota_firmware_api.h"

void firmware_slot_a_main(void)
{
    printf("[FW A] Booting stable firmware A\n");
    confirm_boot_success();
    printf("[FW A] Running normally...\n");
}
