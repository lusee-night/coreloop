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

extern enum cmd_format cdi_format;
extern void *cdi_in;
extern void *cdi_out;

#endif //LN_CORELOOP_CDI_OPTIONS_H
