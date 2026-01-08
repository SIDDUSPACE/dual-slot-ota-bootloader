#include "ota_metadata.h"
#include "memutils.h"
#define printf(...) do {} while (0)
/* Simulated persistent storage */
static ota_metadata_t stored_metadata;

uint32_t ota_metadata_compute_crc(const ota_metadata_t *meta)
{
    const uint8_t *data = (const uint8_t *)meta;
    uint32_t crc = 0;

    uint32_t len = sizeof(ota_metadata_t) - sizeof(uint32_t);
    for (uint32_t i = 0; i < len; i++){
        crc ^= data[i];
    } // exclude crc field
    return crc;
}

bool ota_metadata_load(ota_metadata_t *meta)
{
    memcpy(meta, &stored_metadata, sizeof(ota_metadata_t));

    uint32_t calc_crc = ota_metadata_compute_crc(meta);
    if (calc_crc != meta->crc)
        return false;

    if (meta->active_slot > SLOT_B || meta->pending_slot > SLOT_B)
        return false;

    if (meta->state > OTA_STATE_ROLLBACK)
        return false;
    if (meta->active_slot == meta->pending_slot &&
        meta->state != OTA_STATE_IDLE){
             return false;
        }
       

    return true;
}

bool ota_metadata_save(const ota_metadata_t *meta)
{
    stored_metadata = *meta;
    return true;
}

void ota_metadata_reset(ota_metadata_t *meta)
{
    memset(meta, 0, sizeof(*meta));

    meta->active_slot  = SLOT_A;
    meta->pending_slot = SLOT_B;   // CRITICAL
    meta->state        = OTA_STATE_IDLE;
    meta->boot_attempts = 0;
    meta->firmware_version = 1;

    meta->crc = ota_metadata_compute_crc(meta);
    ota_metadata_save(meta);
}