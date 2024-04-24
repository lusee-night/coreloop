#ifndef GLOBAL_H
#define GLOBAL_H
#include <stdio.h>
#define DDR3_SIZE (1024*1024*(1024+512)) // 1.5GB

#define DEBUG 1
#define debug_print(fmt) \
            do { if (DEBUG) printf(fmt); } while (0);

// Global variables

extern void* DDR3_BASE_ADDR;
extern void* TLM_BUF;
extern void* SPEC_BUF;

void DD3_init();

#endif