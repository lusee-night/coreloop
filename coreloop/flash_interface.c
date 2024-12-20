#include "lusee_commands.h"
#include "spectrometer_interface.h"
#include "core_loop.h"
#include "flash_interface.h"
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "LuSEE_IO.h"
#include "LuSEE_SPI.h"
#include "LuSEE_Flash_cntrl.h"

// FLASH constrol
struct saved_core_state tmp_state;


inline static uint32_t get_flash_addr (uint32_t slot) {
    return slot*PAGES_PER_SLOT*256 + Flash_Recov_Region_1;
}


void flash_state_clear(uint8_t slot) {
   // need to clear just the first 4 bytes;
    uint32_t flash_addr = get_flash_addr(slot);
    SPI_4k_erase_step1(flash_addr);
    mini_wait(2);
    SPI_4k_erase_step2();
    mini_wait(2);
    SPI_4k_erase_step3();
}




void flash_state_store(uint8_t slot, struct core_state* state) {
    debug_print("\r\nStoring state to slot ");
    debug_print_dec(slot);
    debug_print("\r\n");
    tmp_state.in_use = 0xBEBEC;

    tmp_state.state = state;
    /*uint8_t* b = (uint8_t *)(&tmp_state.state);
    for (int i=0;i<sizeof(struct core_state);i++) b[i] = i%0xff;*/

    tmp_state.CRC = CRC(&tmp_state.state,sizeof(struct core_state));
    //debug_print("CRC:");
    //debug_print_hex(tmp_state.CRC);
    //debug_print("\r\n");
    //print_buf(&tmp_state,sizeof(tmp_state));
    uint32_t flash_addr = get_flash_addr(slot);
    void* flash_buf = &tmp_state;
    uint32_t flash_size = sizeof(tmp_state);
    flash_state_clear(slot);
    mini_wait(2);
    while (flash_size>0) {
        uint32_t tocpy = (flash_size<=256) ? flash_size : 256;
        uint8_t *src = (uint8_t*) flash_buf;
        uint8_t *tgt = (uint8_t*) SFL_WR_BUFF;
        for (int i=0; i<tocpy; i++) tgt[i]=src[i];
        mini_wait(2);
        SPI_write_page_step1(flash_addr);
        mini_wait(2);
        SPI_write_page_step2();
        mini_wait(2);
        SPI_write_page_step3();
        if (flash_size>256) flash_size-=256; else flash_size=0;
        flash_addr+=256;
        flash_buf+=256;
    }
}


void  Read_Flash_uC (uint32_t size) {
    // cannot reuse flash_size, since this would trigger write in the interrupt!!
    while (size>0) {
        SPI_read_page(*flash_addr);  //opcode 03h  read page
        uint32_t tocpy = (size<=256) ? size : 256;
        memcpy (flash_buf, SFL_RD_BUFF, tocpy);
        if (size>256) size-=256; else size=0;
        (*flash_addr) += 256;
        (*flash_buf) += 256;
    }
}


bool flash_state_restore(uint8_t slot, struct core_state* state) {
    // try to restore state from flash
    // return true if successful 
    //memset(&tmp_state, 0,  sizeof(tmp_state));
    uint32_t flash_addr = get_flash_addr(slot);
    void* flash_buf = &tmp_state;
    Read_Flash_uC(sizeof(uint32_t),&flash_addr, &flash_buf); // first read just a little bit
    //debug_print ("in use:")
    //debug_print_hex (tmp_state.in_use);
    //debug_print ("\r\n")
    if (tmp_state.in_use == 0xBEBEC) {
        debug_print("\r\nFound an occuped slot ");
        debug_print_dec(slot);

        uint32_t flash_addr = get_flash_addr(slot);
        flash_buf = &tmp_state;
        Read_Flash_uC(sizeof(tmp_state),&flash_addr, &flash_buf);
        //print_buf(&tmp_state,sizeof(tmp_state));

        uint32_t crc = CRC(&tmp_state.state,sizeof(struct core_state));
        //debug_print_hex(tmp_state.CRC);
        //debug_print (" ");
        //debug_print_hex(crc);

        if (crc == tmp_state.CRC) {
         // VICTORY
            debug_print("\r\nCRC match, buying ");
            *state = tmp_state.state;
            return true;
        }
        debug_print("\r\n CRC fail ?!");
        state->base.errors |= FLASH_CRC_FAIL;
        return false;
     }
    return false;
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
            flash_state_clear(i);
        }
        return;
    }

    flash_store_pointer = 0;
    while (!flash_state_restore(flash_store_pointer, state)) {
        flash_store_pointer++;
        if (flash_store_pointer == MAX_STATE_SLOTS) {
            // ideally start with a random store to avoid flash wear, but at this point we have nothing.
            flash_store_pointer = 0;
            return;
        }
    }
    debug_print("Restored existing state from slot ")
    debug_print_dec(flash_store_pointer);
    debug_print("\r\n");
    if (state->base.spectrometer_enable) {
        RFS_start(state);
    }
    
}

