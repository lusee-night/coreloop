#include "LuSEE_IO.h"
#include "LuSEE_SPI.h"
#include "lusee_appIds.h"

#include "lusee_appIds.h"
#include "core_loop.h"
#include "calibrator.h"
#include "calibrator_interface.h"
#include "cdi_interface.h"
#include "flash_interface.h"
#include <stdlib.h>
#include <stdint.h>
#include <string.h>



inline static uint32_t get_flash_addr (uint32_t slot) {
    return slot*4096 + Flash_FS_Save;
}

inline static uint32_t get_cal_flash_addr (uint32_t slot) {
    return slot*4096 + Flash_CAL_Save;
}



void flash_state_store(uint8_t slot, struct core_state* state) {
    // we will do this when spectrometer is off, so ok
    struct saved_state *tostore = (struct saved_state *)(FLASH_WORK);
    tostore->in_use = 0xBEBEC;
    for (int i=0; i<CMD_BUFFER_SIZE; i++) {
        tostore->cmd_arg_high[i] = state->cmd_arg_high[i];
        tostore->cmd_arg_low[i] = state->cmd_arg_low[i];
    }
    tostore->cmd_ptr = state->cmd_ptr;
    tostore->cmd_end = state->cmd_end;
    tostore->CRC = CRC(tostore,sizeof(struct saved_state)-sizeof(uint32_t));
    uint32_t flash_addr = get_flash_addr(slot);
    memcpy_to_flash(flash_addr, tostore, sizeof(struct saved_state));
}


bool flash_slot_busy(uint8_t slot) {
    struct saved_state *tostore = (struct saved_state *)(FLASH_WORK);
    uint32_t flash_addr = get_flash_addr(slot);
    // read just a little bit to start with
    memcpy_from_flash(tostore, flash_addr, 8);
    if (tostore->in_use == 0xBEBEC) {
        return true;
    }
    return false;
}


bool flash_state_restore(uint8_t slot, struct core_state* state) {
    // we will do this when spectrometer is off, so ok
    if (!flash_slot_busy(slot)) {
        return false;
    }
    struct saved_state *tostore = (struct saved_state *)(FLASH_WORK);
    uint32_t flash_addr = get_flash_addr(slot);
    memcpy_from_flash(tostore, flash_addr, sizeof(struct saved_state));
    uint32_t crc = CRC(tostore,sizeof(struct saved_state)-sizeof(uint32_t));
    if (crc == tostore->CRC) {
        for (int i=0; i<CMD_BUFFER_SIZE; i++) {
            state->cmd_arg_high[i] = tostore->cmd_arg_high[i];
            state->cmd_arg_low[i] = tostore->cmd_arg_low[i];
        }
        state->cmd_ptr = tostore->cmd_ptr;
        state->cmd_end = tostore->cmd_end;
        return true;
    } else {
        state->base.errors |= FLASH_CRC_FAIL;
    }
    return false;
 }

void clear_current_slot (struct core_state* state) {

    if (state->flash_slot != -1) {
        debug_print("CLR ");
        debug_print_dec(state->flash_slot)    
        uint32_t flash_addr = get_flash_addr(state->flash_slot);
        SPI_4k_erase(flash_addr);
        state->flash_slot = -1;
    }
}

void store_state (struct core_state* state) {
    // first just make sure we delete 
    clear_current_slot(state);
    // get slote from timer to make sure it is random
    uint16_t timer_time_16;
    uint32_t timer_time_32;
    spec_get_time(&timer_time_32, &timer_time_16);
    state->flash_slot  = ((timer_time_32 >> 4) % MAX_STATE_SLOTS);
    debug_print("STR ");
    debug_print_dec(state->flash_slot)    
    flash_state_store(state->flash_slot, state);
    debug_print_dec(state->flash_slot);
}



void restore_state(struct core_state* state) {
    uint32_t arg1 = spec_read_uC_register(0);  // register contains argumed passed from bootloader
    // now that we have it, delete it
    spec_write_uC_register(0,0);
    if (arg1 == 1) {
        debug_print("\r\nISS\r\n");
        state->flash_slot = -1;
        return;
    } else if (arg1==2) {
        // remove all slots
        debug_print("\r\nDS ");
        for (int i=0; i<MAX_STATE_SLOTS; i++) {
            if (flash_slot_busy(i)) {            
                debug_print_dec(i);
                uint32_t flash_addr = get_flash_addr(i);
                SPI_4k_erase(flash_addr);    
            }
        }
        state->flash_slot = -1;
        return;
    }

    state->flash_slot = 0;
    while (!flash_state_restore(state->flash_slot, state)) {
        state->flash_slot++;
        if (state->flash_slot == MAX_STATE_SLOTS) {
            state->flash_slot = -1;
            return;
        }
    }
    debug_print("\r\nRSS ")
    debug_print_dec(state->flash_slot);
    // now also send the appropriate CDI packet.
    struct state_recover_notification* srn = (struct state_recover_notification*)TLM_BUF;
    srn->slot = state->flash_slot;
    srn->size = state->cmd_end - state->cmd_ptr;
    cdi_dispatch_uC(&state->cdi_stats, AppID_uC_Restored, sizeof(*srn));
    debug_print("\r\n");
}



void flash_calweights_store(uint8_t slot) {
    struct saved_calibrator_weights *tostore = (struct saved_calibrator_weights *)(FLASH_WORK);
    tostore->in_use = 0xBABAC;
    for (int i=0; i<512; i++) {
        tostore->weights[i] = calib_get_weight(i);
    }
    tostore->CRC = CRC(&tostore->weights, 512*sizeof(uint16_t));
    uint32_t flash_addr = get_cal_flash_addr(slot);
    memcpy_to_flash(flash_addr, tostore, sizeof(struct saved_calibrator_weights));
}

bool flash_calweights_restore(uint8_t slot, bool just_check) {
    struct saved_calibrator_weights *tostore = (struct saved_calibrator_weights *)(FLASH_WORK);
    uint32_t flash_addr = get_cal_flash_addr(slot);
    // read just a little bit to start with
    memcpy_from_flash(tostore, flash_addr, 8);
    if (tostore->in_use == 0xBABAC) {
        memcpy_from_flash(tostore, flash_addr, sizeof(struct saved_calibrator_weights));
        uint32_t crc = CRC(&tostore->weights, 512*sizeof(uint16_t));
        if (crc == tostore->CRC) {
            if (!just_check) {
                for (int i=0; i<512; i++) {
                    calib_set_weight(i, tostore->weights[i]);
                }
            }
            debug_print("[FWR]")
            return true;
        }
    }
    debug_print("[FWR fail]");
    return false;
}


void region_get_info(uint32_t region, bool *valid, uint32_t *size, uint32_t *checksum) {
    if ((region<1)||(region>6)) {
        *valid = false;
        *size = 0;
        *checksum = 0xFFFFFFFF;        
        return;
    }
    memcpy_from_flash((void *)FLASH_WORK, (uint32_t)(Flash_Region_1  + (region-1)*Flash_Region_Size + Flash_Meta_Offset), 12);
    uint32_t *ptr = (uint32_t *)(FLASH_WORK);
    *valid = (region == ptr[0]);
    *size = ptr[1];
    *checksum = ptr[2];
}

void region_set_info(uint32_t region, uint32_t size, uint32_t checksum, bool enable) {
    if ((region<1)||(region>6)) {
        return;
    }    
    uint32_t *ptr = (uint32_t *)(FLASH_WORK);
    ptr[0] = enable ? (uint32_t) region : 0;
    ptr[1] = size;
    ptr[2] = checksum;
    memcpy_to_flash((uint32_t)(Flash_Region_1  + (region-1)*Flash_Region_Size + Flash_Meta_Offset), (void *)FLASH_WORK, 12);
}

void flash_region_enable (int region, bool enable){
    bool valid;
    uint32_t size, checksum;
    region_get_info(region, &valid, &size, &checksum);
    // we only change it if needed
    if (enable != valid) region_set_info(region, size, checksum, enable);    
}





void region_check_checksum(uint32_t region, bool* valid, uint32_t *size, uint32_t* info_checksum, uint32_t* data_checksum) {
    if ((region<1) || (region>6 )) {
        *info_checksum = 0;
        *data_checksum = 0xFFFFFFFF;
        *valid = false;
        *size = 0 ;
        return;
    }
    region_get_info(region, valid, size, info_checksum);
    if ((*size==0) || (*size > Flash_Meta_Offset/4)) {
        *data_checksum = 0xFFFFFFFF;
        *valid = false;
        return;
    }

    memcpy_from_flash((void *)FLASH_WORK, (uint32_t)(Flash_Region_1  + (region-1)*Flash_Region_Size), (*size)*4); // size is in words
    uint32_t *ptr = (uint32_t *)(FLASH_WORK);
    uint32_t checksum = 0;
    for (int i=0; i<*size;i++) checksum+= ptr[i];
    *data_checksum = ~checksum + 1;
}

void region_copy_region (int region_src, int region_tgt, struct flash_copy_report_t *report){
    if ((region_src<1) || (region_src>6 ) || (region_tgt<1) || (region_tgt>6)) {
        report->status = FLASH_BAD_REGIONS;
        return;
    }
    report->region_1 = region_src;
    report->region_2 = region_tgt;
    bool valid;
    region_check_checksum(region_src, &valid, &report->size_1, &report->checksum_1_meta, &report->checksum_1_data);
    if ((report->size_1==0) || (report->size_1 > Flash_Meta_Offset/4) || (report->checksum_1_meta != report->checksum_1_data)) {
        report->status = FLASH_COPY_BAD_CHECKSUM_IN;
        return;
    }
    // ok, we can copy
    debug_print_hex (report->size_1);
    debug_print(" ");
    debug_print_hex(report->checksum_1_data);
    debug_print(" ");
    debug_print_hex(report->checksum_1_meta);

    memcpy_to_flash ((uint32_t)(Flash_Region_1  + (region_tgt-1)*Flash_Region_Size), (void *)(FLASH_WORK), (report->size_1)*4);
    debug_print("done writing")
    region_set_info (region_tgt, report->size_1, report->checksum_1_data, true);
    debug_print("done setting info")
    region_check_checksum(region_tgt, &valid, &report->size_2, &report->checksum_2_meta, &report->checksum_2_data);
    if (report->checksum_2_meta != report->checksum_2_data) {
        report->status = FLASH_COPY_BAD_CHECKSUM_OUT;
        return;
    }
    debug_print("done");
        debug_print_hex (report->size_2);
    debug_print(" ");
    debug_print_hex(report->checksum_2_data);
    debug_print(" ");
    debug_print_hex(report->checksum_2_meta);
    report->status = FLASH_COPY_SUCCESS;
}

void flash_send_region_info(struct core_state *state) {
    struct housekeeping_data_100 *data = (struct housekeeping_data_100 *)TLM_BUF;
    update_time(state);
    wait_for_cdi_ready();
    data->base.version = VERSION_ID;
    new_unique_packet_id(state);
    data->base.unique_packet_id = state->unique_packet_id;
    data->base.errors = state->base.errors;
    data->base.housekeeping_type = 100;
    for (int i=0; i<6; i++) {
        bool valid; 
        region_check_checksum(i+1, &valid, &data->size[i], &data->checksum_meta[i], &data->checksum_data[i]);
        data->meta_valid[i] = valid;
    }
    cdi_dispatch_uC(&(state->cdi_stats),AppID_uC_Housekeeping, sizeof(struct housekeeping_data_100));
}

void flash_copy_region_cmd(struct core_state *state, int region_src, int region_tgt) {
    struct housekeeping_data_101 *data = (struct housekeeping_data_101 *)TLM_BUF;
    update_time(state);
    wait_for_cdi_ready();
    data->base.version = VERSION_ID;
    new_unique_packet_id(state);
    data->base.unique_packet_id = state->unique_packet_id;
    data->base.errors = state->base.errors;
    data->base.housekeeping_type = 101;
    region_copy_region(region_src, region_tgt, &data->report);
    cdi_dispatch_uC(&(state->cdi_stats),AppID_uC_Housekeeping, sizeof(struct housekeeping_data_101));
}


