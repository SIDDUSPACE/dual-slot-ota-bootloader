#ifndef OTA_UPDATER_H
#define OTA_UPDATER_H

#include <stdint.h>
#include <stdbool.h>

bool ota_perform_update(uint32_t new_version);

#endif
