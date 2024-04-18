#ifndef CDI_INTERFACE_H
#define CDI_INTERFACE_H

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include "cdi_interface.h"

extern void* CDI_BASE_ADDR;

void cdi_init();
// returns true if a new command is available, and sets the command and arguments
bool cdi_new_command(uint8_t *cmd, uint8_t *arg_high, uint8_t *arg_low );

// returns true if the CDI is ready to accept new data  (i.e. write to staging area and dispatching it)
bool cdi_ready();
// dispatches the CDI data packet of a given size with the right appID
// AS : add message type (0x20 - 0x27)
void cdi_dispatch (uint16_t appID, uint32_t length);



#endif