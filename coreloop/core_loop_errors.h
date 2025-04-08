#ifndef CORELOOP_ERRORS_H
#define CORELOOP_ERRORS_H


// unknown CDI command received
#define CDI_COMMAND_UNKNOWN 1
// CDI command called at wrong time (i.e. program sequencer while it is running)
#define CDI_COMMAND_BAD 2
// CDI command called with wrong arguments
#define CDI_COMMAND_BAD_ARGS 4
// CDI command lost due to FW FIFO overflow
#define CDI_COMMAND_LOST 8
// CDI internal bufer overlow
#define CDI_COMMAND_BUFFER_OVERFLOW 16
// Cannot autogain
#define ANALOG_AGC_TOO_HIGH_CH0 32
#define ANALOG_AGC_TOO_HIGH_CH1 64
#define ANALOG_AGC_TOO_HIGH_CH2 128
#define ANALOG_AGC_TOO_HIGH_CH3 256

#define ANALOG_AGC_TOO_LOW_CH0 512
#define ANALOG_AGC_TOO_LOW_CH1 1024
#define ANALOG_AGC_TOO_LOW_CH2 2048
#define ANALOG_AGC_TOO_LOW_CH3 4096

#define ANALOG_AGC_ACTION_CH0 (1<<13)
#define ANALOG_AGC_ACTION_CH1 (1<<14)
#define ANALOG_AGC_ACTION_CH2 (1<<15)
#define ANALOG_AGC_ACTION_CH3 (1<<16)

#define DF_SPECTRA_DROPPED (1<<17)
#define DF_CAL_DROPPED (1<<18)
#define FLASH_CRC_FAIL (1<<19)
#define FPGA_TEMP_HIGH (1<<20)

#define DIGITAL_AGC_ACTION_CH0 (1<<21)
#define DIGITAL_AGC_ACTION_CH1 (1<<22)
#define DIGITAL_AGC_ACTION_CH2 (1<<23)
#define DIGITAL_AGC_ACTION_CH3 (1<<24)
#define DIGITAL_AGC_STUCK      (1<25)



#define INTERNAL_ERROR (1<<31)




#endif
