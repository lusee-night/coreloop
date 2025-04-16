#ifndef WATCHDOGS_H
#define WATCHDOGS_H

#include "core_loop.h"
#include <stdint.h>
#include <stdbool.h>

void send_watchdog_packet(struct core_state* state, uint8_t tripped);
bool process_watchdogs(struct core_state* state);

#endif

