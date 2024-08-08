#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "LuSEE_IO.h"
#include "spectrometer_interface.h"
#include "cdi_interface.h"
#include "core_loop.h"
#include "cdi_options.h"
#include "main.h"

enum cmd_format cdi_format = UNSPECIFIED;
void *cdi_in;
void *cdi_out;

int main(int argc, char *argv[]) {
    int opt;
    while ((opt = getopt(argc, argv, "fpi:o:")) != -1) {
        switch (opt) {
            case 'f':
                if (cdi_format != UNSPECIFIED) {
                    raiseError("Must specify either file mode or port mode, but not both\n", argv);
                }
                cdi_format = CMD_FILE;
                break;
            case 'p':
                if (cdi_format != UNSPECIFIED) {
                    raiseError("Must specify either file mode or port mode, but not both\n", argv);
                }
                cdi_format = CMD_PORT;
                break;
            case 'i':
                cdi_in = optarg;
                break;
            case 'o':
                cdi_out = optarg;
                break;
            default:
                raiseError("", argv);
                break;
        }
    }
    if (cdi_format == UNSPECIFIED) {
        raiseError("Must specify either file mode or port mode, but not both\n", argv);
    }
    // #TODO: port numbers should be ints or just strings?
    if (cdi_format == CMD_PORT) {
        int in_int = atoi(cdi_in);
        int out_int = atoi(cdi_out);
        cdi_in = &in_int;
        cdi_out = &out_int;
    }

    spectrometer_init();
    cdi_init();
    DDR3_init();

    /*
     * TODO: pass input, output, and format into proper coreloop section input & output will probably need to be passed
     * as void* types and then cast appropriately based on the value of format, if port numbers are ints
     */
    core_loop();


    return 0;

}

void raiseError(char *str, char *argv[]) {
    fprintf(stderr, "%s"
                    "Usage: %s -[f|p] -i [file|port] -o [file|port]\n", str, argv[0]);
    exit(EXIT_FAILURE);
}
