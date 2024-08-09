//
// Created by jrsyp on 8/7/2024.
//

#ifndef LN_CORELOOP_MAIN_H
#define LN_CORELOOP_MAIN_H

#include <string.h>

// TODO: write default values
#define DEFAULT_PORT_IN  32100
#define DEFAULT_PORT_OUT 32101
#define DEFAULT_FILE_IN  "data/commands.dat"
#define DEFAULT_FILE_OUT "data/cdi_output"

void raiseError(char *str, char *argv[]);

#endif //LN_CORELOOP_MAIN_H
