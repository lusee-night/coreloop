//
// Created by jrsyp on 8/8/2024.
//

#ifndef LN_CORELOOP_CDI_OPTIONS_H
#define LN_CORELOOP_CDI_OPTIONS_H

enum cmd_format {
    UNSPECIFIED,
    CMD_FILE,
    CMD_PORT,
};

union cdi_dtype {
    char *file;
    int port;
};

extern enum cmd_format cdi_format;
extern union cdi_dtype cdi_in;
extern union cdi_dtype cdi_out;

#endif //LN_CORELOOP_CDI_OPTIONS_H
