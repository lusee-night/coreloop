#ifndef GLOBAL_H
#define GLOBAL_H
#include <stdio.h>
#include <inttypes.h>

#define DDR3_SIZE (1024*1024*(1024+512)) // 1.5GB

#define DEBUG 1
#define debug_print(fmt) \
            do { if (DEBUG) printf(fmt); } while (0);

// Global variables

// timer stuff
extern int g_core_timer_0;
#define EXT_IRQ_KEEP_ENABLED 1
uint8_t MSYS_EI4_IRQHandler(void);
void TMR_clear_int(int* );


// address stuff
extern void* DDR3_BASE_ADDR;
extern void* SPEC_TICK;
extern void* SPEC_TOCK;
extern void* TLM_BUF;
extern void* SPEC_BUF;

void DDR3_init();

#endif