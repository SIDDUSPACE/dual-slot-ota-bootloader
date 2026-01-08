#pragma once

typedef enum {
    OTA_STATE_NORMAL = 0,
    OTA_STATE_UPDATE_PENDING,
    OTA_STATE_TESTING,
    OTA_STATE_ROLLBACK
} ota_state_t;

typedef struct {
    ota_state_t state;
    int active_slot;   // 0 = A, 1 = B
} ota_metadata_t;

extern ota_metadata_t meta;

void ota_metadata_save(void);
