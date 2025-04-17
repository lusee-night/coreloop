#ifndef GLOBAL_H
#define GLOBAL_H
#include <stdio.h>
#include <stdbool.h>
#include <inttypes.h>
#include <time.h>

#define DDR3_SIZE (1024*1024*(1024+512)) // 1.5GB

#define DEBUG 1
#define debug_print(fmt) \
            do { if (DEBUG) {printf(fmt); fflush(stdout);} } while (0);

#define debug_print_dec(fmt) \
            do { if (DEBUG) printf("%i", fmt); } while (0);

#define debug_print_hex(fmt) \
            do { if (DEBUG) printf("%x",fmt); } while (0);


// Global variables

// timer stuff
extern int g_core_timer_0;
extern struct timespec time_start, time_now;
#define EXT_IRQ_KEEP_ENABLED 1
uint8_t MSYS_EI4_IRQHandler(void);
void TMR_clear_int(int* time);


// address stuff
extern void* DDR3_BASE_ADDR;
extern void* SPEC_TICK;
extern void* SPEC_TOCK;
extern void* FLASH_WORK; 
extern void* TR_SPEC_TICK;
extern void* TR_SPEC_TOCK;
extern void* GRIMM_SPEC_TICK;
extern void* GRIMM_SPEC_TOCK;
extern void* TLM_BUF;
extern void* SPEC_BUF;
extern void* CAL_DF;
extern void* CAL_DATA;

extern const size_t SPEC_DATA_SIZE;
extern const size_t TR_SPEC_DATA_SIZE;

void DDR3_init();

static inline void* spectra_write_buffer(bool tick_tock)    { return tick_tock ? SPEC_TICK    : SPEC_TOCK; }
static inline void* spectra_read_buffer(bool tick_tock)     { return tick_tock ? SPEC_TOCK    : SPEC_TICK; };
static inline void* tr_spectra_write_buffer(bool tick_tock) { return tick_tock ? TR_SPEC_TICK : TR_SPEC_TOCK; }
static inline void* tr_spectra_read_buffer(bool tick_tock)  { return tick_tock ? TR_SPEC_TOCK : TR_SPEC_TICK; };
static inline void* grimm_spectra_write_buffer(bool tick_tock) { return tick_tock ? GRIMM_SPEC_TICK : GRIMM_SPEC_TOCK; }
static inline void* grimm_spectra_read_buffer(bool tick_tock)  { return tick_tock ? GRIMM_SPEC_TOCK : GRIMM_SPEC_TICK; };


#endif