#include <stdint.h>


// FLASH constrol
extern volatile uint32_t flash_clear;
extern volatile uint32_t flash_write;
extern volatile uint32_t flash_addr;
extern void* flash_buf;
extern volatile uint32_t flash_size;
extern volatile uint32_t flash_wait;


// flash operations
void flash_state_store(uint8_t slot);
bool flash_state_restore(uint8_t slot);
void flash_state_clear(uint8_t slot);
void restore_state();