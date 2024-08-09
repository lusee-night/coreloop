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
volatile uint32_t flash_clear;
volatile uint32_t flash_write;
volatile uint32_t flash_addr;
void* flash_buf;
volatile uint32_t flash_size;
volatile uint32_t flash_wait;

struct saved_core_state tmp_state;


void set_flash_addr (uint32_t slot) {
    flash_addr = slot*PAGES_PER_SLOT*256 + Flash_Recov_Region_1;
}


void flash_state_clear(uint8_t slot) {
   // need to clear just the first 4 bytes;
    set_flash_addr(slot);
    flash_clear = 4;
}




void flash_state_store (uint8_t slot) {
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
    set_flash_addr(slot);
    flash_buf = &tmp_state;
    flash_size = sizeof(tmp_state);
    flash_clear = 4;
}


void  Read_Flash_uC (uint32_t size) {
    // cannot reuse flash_size, since this would trigger write in the interrupt!!
    while (size>0) {
        //debug_print("flash read ")
        //debug_print_dec(size);
        //debug_print (" ");
        //debug_print_hex(flash_addr);
        //debug_print("\r\n");

        SPI_read_page(flash_addr);  //opcode 03h  read page

        uint32_t tocpy = (size<=256) ? size : 256;
        memcpy (flash_buf, SFL_RD_BUFF, tocpy);
        if (size>256) size-=256; else size=0;
        flash_addr += 256;
        flash_buf += 256;
    }
}


bool flash_state_restore(uint8_t slot) {
    // try to restore state from flash
    // return true if successful 
    //memset(&tmp_state, 0,  sizeof(tmp_state));
    set_flash_addr(slot);
    flash_buf = &tmp_state;
    Read_Flash_uC(sizeof(uint32_t)); // first read just a little bit
    //debug_print ("in use:")
    //debug_print_hex (tmp_state.in_use);
    //debug_print ("\r\n")
    if (tmp_state.in_use == 0xBEBEC) {
        debug_print("\r\nFound an occuped slot ");
        debug_print_dec(slot);

        set_flash_addr(slot);
        flash_buf = &tmp_state;
        Read_Flash_uC(sizeof(tmp_state));
        //print_buf(&tmp_state,sizeof(tmp_state));

        uint32_t crc = CRC(&tmp_state.state,sizeof(struct core_state));
        //debug_print_hex(tmp_state.CRC);
        //debug_print (" ");
        //debug_print_hex(crc);

        if (crc == tmp_state.CRC) {
         // VICTORY
            debug_print("\r\nCRC match, buying ");
            state = tmp_state.state;
            return true;
        }
        debug_print("\r\n CRC fail ?!");void debug_helper(uint8_t arg);
        state.base.errors |= FLASH_CRC_FAIL;
        return false;
     }
    return false;
 }

void restore_state() {
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
            while (flash_clear>0) {}
        }
        return;
    }

    flash_store_pointer == 0;
    while (!flash_state_restore(flash_store_pointer)) {
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
    if (state.base.spectrometer_enable) {
        RFS_start();
    }
    
}
