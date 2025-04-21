#include "LuSEE_IO.h"
#include <stdlib.h>
#include <stdint.h>

struct timespec time_start, time_now;

void* DDR3_BASE_ADDR;
void *SPEC_TICK, *SPEC_TOCK, *TR_SPEC_TICK, *TR_SPEC_TOCK, *GRIMM_SPEC_TICK, *GRIMM_SPEC_TOCK;
void *FLASH_WORK;
void *CAL_DF, *CAL_DATA;
int g_core_timer_0;

const size_t SPEC_DATA_SIZE = 2048*16*sizeof(uint32_t);
const size_t TR_SPEC_DATA_SIZE = 2048*16*sizeof(uint32_t);

void DDR3_init() {
    DDR3_BASE_ADDR = malloc(DDR3_SIZE);
    SPEC_TICK = DDR3_BASE_ADDR;  // size = 2048*16*sizeof(uint32_t);
    SPEC_TOCK = SPEC_TICK + SPEC_DATA_SIZE;
    TR_SPEC_TICK = SPEC_TOCK + SPEC_DATA_SIZE;
    TR_SPEC_TOCK = TR_SPEC_TICK + TR_SPEC_DATA_SIZE;
    CAL_DATA = TR_SPEC_TOCK + TR_SPEC_DATA_SIZE;
    GRIMM_SPEC_TICK = malloc(32*1024);
    GRIMM_SPEC_TICK = malloc(32*1024);
    FLASH_WORK = malloc(64*1024);
}

void TMR_clear_int(int* time) {};