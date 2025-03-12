#ifndef WATCHDOGS_H
#define WATCHDOGS_H

#include "core_loop.h"
#include <stdint.h>

uint8_t check_and_handle_watchdogs(struct core_state* state);

#endif

