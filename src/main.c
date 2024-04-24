#include <stdio.h>
#include "LuSEE_IO.h"

#include "spectrometer_interface.h"
#include "cdi_interface.h"
#include "core_loop.h"

int main() {

    spectrometer_init();
    cdi_init();
    DDR3_init();
    
    core_loop();


    return 0;

}
