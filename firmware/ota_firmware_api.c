#include "ota_metadata.h"
#include "ota_firmware_api.h"
#include <stdio.h>

void confirm_boot_success(void)
{
    ota_metadata_t meta;

    if (!ota_metadata_load(&meta)) {
        printf("[FW] Metadata invalid during confirmation\n");
        return;
    }

    if (meta.state == OTA_STATE_TESTING) {
        printf("[FW] Boot confirmed successfully\n");
        meta.state = OTA_STATE_CONFIRMED;
        meta.boot_attempts = 0;
        meta.crc = ota_metadata_compute_crc(&meta);
        ota_metadata_save(&meta);
    }
}
