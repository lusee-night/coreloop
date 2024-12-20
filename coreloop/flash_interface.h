#include <stdint.h>



// flash operations
void flash_state_store(uint8_t slot, struct core_state*);
bool flash_state_restore(uint8_t slot, struct core_state*);
void flash_state_clear(uint8_t slot);
void restore_state();