#ifndef OTA_METADATA_H
#define OTA_METADATA_H

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    SLOT_A = 0,
    SLOT_B = 1
} slot_t;

typedef enum {
    OTA_STATE_IDLE = 0,
    OTA_STATE_UPDATE_PENDING,
    OTA_STATE_TESTING,
    OTA_STATE_CONFIRMED,
    OTA_STATE_ROLLBACK
} ota_state_t;

typedef struct {
    slot_t active_slot;
    slot_t pending_slot;
    ota_state_t state;
    uint32_t boot_attempts;
    uint32_t firmware_version;
    uint32_t crc;
} ota_metadata_t;

bool ota_metadata_load(ota_metadata_t *meta);
bool ota_metadata_save(const ota_metadata_t *meta);
void ota_metadata_reset(ota_metadata_t *meta);
uint32_t ota_metadata_compute_crc(const ota_metadata_t *meta);

#endif
