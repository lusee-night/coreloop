#include "LuSEE_IO.h"
#include <stdlib.h>

void* DDR3_BASE_ADDR;    

void DDR3_init() {
    DDR3_BASE_ADDR = malloc(DDR3_SIZE);
}