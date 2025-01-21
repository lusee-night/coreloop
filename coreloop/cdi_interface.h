//
//

#ifndef LN_CORELOOP_CDI_INTERFACE_H
#define LN_CORELOOP_CDI_INTERFACE_H

#include <stdbool.h>
#include <stdint.h>

// cdi interface section
void cdi_init();
// fills the command buffer with the next command from the CDI 
// ONLY ON TESTING HARNESS, in reality this happens by the underlying hardware
void cdi_fill_command_buffer();

// returns true if a new command is available, and sets the command and arguments
bool cdi_new_command(uint8_t *cmd, uint8_t *arg_high, uint8_t *arg_low );

// returns the number of commands received
uint32_t cdi_command_count();

// returns the total number of commands received
uint32_t cdi_total_command_count();

// returns true if the CDI is ready to accept new data  (i.e. write to staging area and dispatching it)
bool cdi_ready();
// waits until CDI buffers is ready to be written.
void wait_for_cdi_ready();
// dispatches the CDI data packet of a given size with the right appID
// AS : add message type (0x20 - 0x27)
void cdi_dispatch (uint16_t appID, uint32_t length);

#endif //LN_CORELOOP_CDI_INTERFACE_H
