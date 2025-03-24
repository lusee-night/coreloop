#include <stdint.h>



// higher interface
void store_state (struct core_state* state);
void restore_state(struct core_state* state);

// lower level flash operations
void flash_state_store(uint8_t slot, struct core_state*);
bool flash_state_restore(uint8_t slot, struct core_state*);
