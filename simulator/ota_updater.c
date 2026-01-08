#include "ota_updater.h"
#include "ota_metadata.h"
#include <stdio.h>
#include "fault_injection.h"
bool ota_perform_update(uint32_t new_version)
{
    ota_metadata_t meta;

    if (!ota_metadata_load(&meta)) {
        printf("[OTA] Metadata invalid. Abort update.\n");
        return false;
    }

#if FAULT_POWER_LOSS_DURING_OTA
    printf("[FAULT] Power loss during firmware write!\n");
    return false;   // OTA stops BEFORE metadata update
#endif

    if (new_version <= meta.firmware_version) {
        printf("[OTA] Downgrade attempt blocked\n");
        return false;
    }

    slot_t inactive_slot =
        (meta.active_slot == SLOT_A) ? SLOT_B : SLOT_A;

    printf("[OTA] Writing firmware to slot %s\n",
           inactive_slot == SLOT_A ? "A" : "B");

    /* Simulated successful write */

    meta.pending_slot = inactive_slot;
    meta.state = OTA_STATE_UPDATE_PENDING;
    meta.firmware_version = new_version;
    meta.boot_attempts = 0;
    meta.crc = ota_metadata_compute_crc(&meta);

    ota_metadata_save(&meta);

    printf("[OTA] Update staged. Reboot required.\n");
    return true;
}