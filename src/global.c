#include "global.h"
#include <stdlib.h>




void* DDR3_BASE_ADDR;    

void DD3_init() {
    DDR3_BASE_ADDR = malloc(DDR3_SIZE);
}