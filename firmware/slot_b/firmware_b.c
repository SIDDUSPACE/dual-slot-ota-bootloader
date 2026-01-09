#include <stdio.h>
#include "ota_firmware_api.h"
#define SIMULATE_FAILURE  0   
void firmware_slot_b_main(void)
{
    printf("[FW B] Booting new firmware B\n");

#if SIMULATE_FAILURE
    printf("[FW B] Simulating hang / failure\n");
    while (1);  
#else
    confirm_boot_success();
    printf("[FW B] Running normally...\n");
#endif
}
