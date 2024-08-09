#include "LuSEE_IO.h"
#include <stdlib.h>
#include <stdint.h>

struct timespec time_start, time_now;

void* DDR3_BASE_ADDR;    
void *SPEC_TICK, *SPEC_TOCK, *SPEC_TIME_RESOLVED;
int g_core_timer_0;

void DDR3_init() {
    DDR3_BASE_ADDR = malloc(DDR3_SIZE);
    SPEC_TICK = DDR3_BASE_ADDR;  // size = 2048*16*sizeof(uint32_t);
    SPEC_TOCK = DDR3_BASE_ADDR + 2048*16*sizeof(uint32_t);  // size = 2048*16*sizeof(uint32_t);  
    SPEC_TIME_RESOLVED = DDR3_BASE_ADDR + 2048*16*sizeof(uint32_t)*2; // size = 2048*16*sizeof(uint16_t)
}

void TMR_clear_int(int* time) {};