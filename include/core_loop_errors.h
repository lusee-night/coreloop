#ifndef CORELOOP_ERRORS_H
#define CORELOOP_ERRORS_H


// unknown CDI command receiver
#define UNKNOWN_CDI_COMMAND 0x01
// CDI command called at wrong time ((i.e. program sequencer while it is running)
#define CDI_COMMAND_WRONG 0x02
// CDI command called with wrong arguments
#define CDI_COMMAND_WRONG_ARGS 0x04


#endif