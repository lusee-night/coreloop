#include "LuSEE_IO.h"
#include <stdlib.h>
#include <stdint.h>

struct timespec time_start, time_now;

void* DDR3_BASE_ADDR;
void *SPEC_TICK, *SPEC_TOCK, *TR_SPEC_TICK, *TR_SPEC_TOCK;
int g_core_timer_0;

const size_t SPEC_DATA_SIZE = sizeof(struct SpectraIn);
const size_t TR_SPEC_DATA_SIZE = NCHANNELS * NSPECTRA * sizeof(uint32_t);

void DDR3_init() {
    DDR3_BASE_ADDR = malloc(DDR3_SIZE);
    SPEC_TICK = DDR3_BASE_ADDR;
    SPEC_TOCK = SPEC_TICK + SPEC_DATA_SIZE;
    TR_SPEC_TICK = SPEC_TOCK + SPEC_DATA_SIZE;
    TR_SPEC_TOCK = TR_SPEC_TICK + TR_SPEC_DATA_SIZE;
}

void TMR_clear_int(int* time) {};