#include <stdio.h>
#include "global.h"

#include "spectrometer_interface.h"
#include "cdi_interface.h"

int main() {
    spectrometer_init();
    cdi_init();
    printf("Hello, World!\n");
    return 0;

}
