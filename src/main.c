#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "LuSEE_IO.h"
#include "spectrometer_interface.h"
#include "core_loop.h"
#include "main.h"

int main(int argc, char *argv[]) {
    int opt;
    enum cmd_format format = UNSPECIFIED;
    while ((opt = getopt(argc, argv, "fpi:o:")) != -1) {
        switch (opt) {
            case 'f':
                if (format != UNSPECIFIED) {

                }
                format = CMD_FILE;
                break;
            case 'p':
                format = CMD_PORT;
                break;
            case 'i':
                break;
            case 'o':
                break;
            default:
                fprintf(stderr, "Usage: %s [-f|p] -i [file|port] -o [file|port]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }
    if (format == UNSPECIFIED) {
        fprintf(stderr, "Must specify either file mode or port mode\n"
                        "Usage: %s -[f|p] -i [file|port] -o [file|port]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    spectrometer_init();
    cdi_init();
    DDR3_init();
    
    core_loop();


    return 0;

}
