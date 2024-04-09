#include <stdio.h>
#include "global.h"

#include "spectrometer_interface.h"
#include "cdi_interface.h"
#include "core_loop.h"

int main() {

    spectrometer_init();
    cdi_init();
    DD3_init();
    
    core_loop();


    return 0;

}
