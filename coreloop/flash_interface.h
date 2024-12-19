#include <stdint.h>



// flash operations
void flash_state_store(uint8_t slot);
bool flash_state_restore(uint8_t slot);
void flash_state_clear(uint8_t slot);
void restore_state();