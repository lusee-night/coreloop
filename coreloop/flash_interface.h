#ifndef FLASH_INTERFACE_H
#define FLASH_INTERFACE_H
#pragma pack(1)

#include <stdint.h>

enum flash_copy_status {
    FLASH_BAD_REGIONS,
    FLASH_COPY_BAD_CHECKSUM_IN,
    FLASH_COPY_BAD_CHECKSUM_OUT,
    FLASH_COPY_SUCCESS
};

struct flash_copy_report_t {
    int region_1;
    int region_2;
    uint32_t size_1, size_2;
    uint32_t checksum_1_meta, checksum_1_data;
    uint32_t checksum_2_meta, checksum_2_data;
    enum flash_copy_status status;
};


// higher interface
void store_state (struct core_state* state);
void restore_state(struct core_state* state);
void clear_current_slot (struct core_state* state);

// lower level flash operations
void flash_state_store(uint8_t slot, struct core_state*);
bool flash_state_restore(uint8_t slot, struct core_state*);

void flash_calweights_store(uint8_t slot);
bool flash_calweights_restore(uint8_t slot, bool just_check);


// region operations
void get_flash_region_info(uint32_t region, uint32_t *size, uint32_t *checksum);
void region_check_checksum(uint32_t region, bool* valid, uint32_t *size, uint32_t* info_checksum, uint32_t* data_checksum);
void region_copy_region (int region_src, int region_tgt, struct flash_copy_report_t *report);

#endif