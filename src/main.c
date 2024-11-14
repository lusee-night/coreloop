#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "LuSEE_IO.h"
#include "spectrometer_interface.h"
#include "cdi_interface.h"
#include "core_loop.h"
#include "cdi_options.h"
#include "main.h"

enum cmd_format cdi_format = CMD_FILE;
struct cdi_dtype cdi_in;
struct cdi_dtype cdi_out;

int main(int argc, char *argv[]) {

    struct core_state global_state;
    
    strcpy(cdi_in.file, DEFAULT_FILE_IN);
    strcpy(cdi_out.file, DEFAULT_FILE_OUT);
    cdi_in.port = DEFAULT_PORT_IN;
    cdi_out.port = DEFAULT_PORT_OUT;
    cdi_format = CMD_FILE;
    
    int opt;
    while ((opt = getopt(argc, argv, "hm:i:o:")) != -1) {
        switch (opt) {
            case 'h':
                fprintf(stdout, "Usage: %s -m [\"file\" | \"port\"] -i [input file/port] -o [output file/port]\n", argv[0]);
                exit(EXIT_SUCCESS);
            case 'm':
                if (!strcmp(optarg, "file") || !strcmp(optarg, "f")) {
                    cdi_format = CMD_FILE;
                } else if (!strcmp(optarg, "port") || !strcmp(optarg, "p")) {
                    cdi_format = CMD_PORT;
                } else {
                    raiseError("Must specify mode -m: f or file for file mode, p or port for port mode\n", argv);
                }
                break;
            case 'i':
                strcpy(cdi_in.file, optarg);
                cdi_in.port = atoi(cdi_in.file); // try to convert to in, don't worry if it fails

                break;
            case 'o':
                strcpy(cdi_out.file, optarg);
                cdi_out.port = atoi(cdi_out.file); // try to convert to in, don't worry if it fails
                break;
            default:
                raiseError("", argv);
                break;
        }
    }
    if (cdi_format == UNSPECIFIED) {
        raiseError("Must specify mode -m: f or file for file mode, p or port for port mode\n", argv);
    }

    spectrometer_init();
    cdi_init();
    DDR3_init();
    clock_gettime(CLOCK_REALTIME, &time_start);
    soft_reset_flag = 0;
    do {
        printf("HERE {soft_reset_flag: %d}\n", soft_reset_flag);
        core_loop(&global_state);
    } while (soft_reset_flag);


    return 0;

}

void raiseError(char *str, char *argv[]) {
    fprintf(stderr, "%s"
                    "Usage: %s -m [f/file | p/port] -i [file|port input] -o [file|port output]\n", str, argv[0]);
    exit(EXIT_FAILURE);
}
