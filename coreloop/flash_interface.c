#include "LuSEE_IO.h"
#include "LuSEE_SPI.h"


#include "lusee_appIds.h"
#include "core_loop.h"
#include "calibrator.h"
#include "calibrator_interface.h"
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

