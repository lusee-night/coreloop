#ifndef CORELOOP_ERRORS_H
#define CORELOOP_ERRORS_H


// unknown CDI command receiver
#define UNKNOWN_CDI_COMMAND 1
// CDI command called at wrong time ((i.e. program sequencer while it is running)
#define CDI_COMMAND_WRONG 2
// CDI command called with wrong arguments
#define CDI_COMMAND_WRONG_ARGS 4
// Cannot autogain
#define ANALOG_AGC_TOO_HIGH 8
#define ANALOG_AGC_TOO_LOW 16



#endif