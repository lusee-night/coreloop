#ifndef CORELOOP_ERRORS_H
#define CORELOOP_ERRORS_H


// unknown CDI command received
#define CDI_COMMAND_UNKNOWN 1
// CDI command called at wrong time ((i.e. program sequencer while it is running)
#define CDI_COMMAND_BAD 2
// CDI command called with wrong arguments
#define CDI_COMMAND_BAD_ARGS 4
// Cannot autogain
#define ANALOG_AGC_TOO_HIGH_CH1 8
#define ANALOG_AGC_TOO_HIGH_CH2 16
#define ANALOG_AGC_TOO_HIGH_CH3 32
#define ANALOG_AGC_TOO_HIGH_CH4 64

#define ANALOG_AGC_TOO_LOW_CH1 128
#define ANALOG_AGC_TOO_LOW_CH2 256
#define ANALOG_AGC_TOO_LOW_CH3 512
#define ANALOG_AGC_TOO_LOW_CH4 1024

#define ANALOG_AGC_ACTION_CH1 (1<<11)
#define ANALOG_AGC_ACTION_CH2 (1<<12)
#define ANALOG_AGC_ACTION_CH3 (1<<13)
#define ANALOG_AGC_ACTION_CH4 (1<<14)

#define DF_SPECTRA_DROPPED (1<<15)
#define FLASH_CRC_FAIL (1<<16)



#endif
