#include "LuSEE_IO.h"
#include "LuSEE_SPI.h"

#include "lusee_appIds.h"
#include "core_loop.h"
#include "flash_interface.h"
#include <stdlib.h>
#include <stdint.h>
#include <string.h>



inline static uint32_t get_flash_addr (uint32_t slot) {
    return slot*4096 + Flash_FS_Save;
}


void flash_state_store(uint8_t slot, struct core_state* state) {
    // we will do this when spectrometer is off, so ok
    struct saved_state *tostore = (struct saved_state *)(SPEC_TICK);
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




bool flash_state_restore(uint8_t slot, struct core_state* state) {
    // we will do this when spectrometer is off, so ok
    struct saved_state *tostore = (struct saved_state *)(SPEC_TICK);
    uint32_t flash_addr = get_flash_addr(slot);
    // read just a little bit to start with
    memcpy_from_flash(tostore, flash_addr, 8);
    if (tostore->in_use != 0xBEBEC) {
        return false;
    }
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
    }
    return false;
 }

void clear_current_slot (struct core_state* state) {

    if (state->flash_slot != -1) {
        debug_print("CLR ");
        debug_print_dec(state->flash_slot)    
        uint32_t flash_addr = get_flash_addr(state->flash_slot);
        SPI_4k_erase(flash_addr);
    }
}

void store_state (struct core_state* state) {
    // first just make sure we delete 
    clear_current_slot(state);
    // get slote from timer to make sure it is random
    uint16_t timer_time_16;
    uint32_t timer_time_32;
    spec_get_time(&timer_time_32, &timer_time_16);
    state->flash_slot  = (timer_time_32 >> 4) % MAX_STATE_SLOTS;
    debug_print("STR ");
    debug_print_dec(state->flash_slot)    
    flash_state_store(state->flash_slot, state);
    debug_print("Stored sequence to slot ")
    debug_print_dec(state->flash_slot);
}



void restore_state(struct core_state* state) {
    uint32_t arg1 = spec_read_uC_register(0);  // register contains argumed passed from bootloader
    if (arg1 == 1) {
        debug_print("Ignoring saved states\r\n");
        return;
    } else if (arg1==2) {
        // remove all slots
        for (int i=0; i<MAX_STATE_SLOTS; i++) {
            debug_print("Deleting slot ");
            debug_print_dec(i);
            debug_print("\r\n");
            uint32_t flash_addr = get_flash_addr(i);
            SPI_4k_erase(flash_addr);
        }
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
    debug_print("Restored sequence from slot ")
    debug_print_dec(state->flash_slot);
    // now also send the appropriate CDI packet.
    struct state_recover_notification* srn = (struct state_recover_notification*)TLM_BUF;
    srn->slot = state->flash_slot;
    srn->size = state->cmd_end - state->cmd_ptr;
    cdi_dispatch_uC(&state->cdi_stats, AppID_uC_Restored, sizeof(*srn));
    debug_print("\r\n");
}

