#include "LuSEE_IO.h"
#include <stdlib.h>
#include <stdint.h>

void* DDR3_BASE_ADDR;    
void *SPEC_TICK, *SPEC_TOCK;
int g_core_timer_0;

void DDR3_init() {
    DDR3_BASE_ADDR = malloc(DDR3_SIZE);
    SPEC_TICK = DDR3_BASE_ADDR;
    SPEC_TOCK = DDR3_BASE_ADDR + 2048*16*sizeof(uint32_t);    
}

void TMR_clear_int(int* time) {};