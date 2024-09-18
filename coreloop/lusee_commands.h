
#ifndef LUSEE_SETTINGS_H
#define LUSEE_SETTINGS_H

         
// Top-level CDI commands         
// Master command for uC control
#define RFS_SETTINGS  0x10 

// Generate outliers in this number
#define CTRL_OUTLIER_NUM  0xA0 

// Generate outliers of this amplitude in units of 1/256
#define CTRL_OUTLIER_AMP  0xA1 

// Generate outliers of this many (sequential) bins
#define CTRL_OUTLIER_BINS  0xA2 

// Wait for 0.1s x argument (intercepted by commander)
#define CTRL_WAIT  0xE0 



// RFS Settings commands
// wait mode - disable data taking
#define RFS_SET_STOP  0x00 

// Start data acquisition. To start anything setup by 0x1x or 0x2x
#define RFS_SET_START  0x01 

// Soft reset, if arg == 0: restore stored cfg, 01 = ignore stored cfg, 02 = delete all stored cfgs
#define RFS_SET_RESET  0x02 

// Stores current configuration
#define RFS_SET_STORE  0x03 

// Recalls configuration from previous store
#define RFS_SET_RECALL  0x04 

// Return housekeeping data, ARG = 0 -- full housekeeping; ARG = 1 ADC statistics;
#define RFS_SET_HK_REQ  0x05 

// Set ADC mode: optionally disable ADCs (bits 0-3 in arg)
#define RFS_SET_DISABLE_ADC  0x06 

// Autorange ADC and then set an ADC packet
#define RFS_SET_RANGE_ADC  0x07 

// Request ADC waform arg contains channel number
#define RFS_SET_WAVEFORM  0x08 

// Wait arg number of ticks (10ms) before processing next CMD (careful with 64 buffer!)
#define RFS_SET_WAIT_TICKS  0x09 

// Wait arg number of seconds before processing next CMD (careful with 64 buffer!)
#define RFS_SET_WAIT_SECS  0x0A 

// Debug command (used only in debugging)
#define RFS_SET_DEBUG  0x0D 

// prepare for power cut -- mode announcing power cut 5 seconds after issue
#define RFS_SET_TIME_TO_DIE  0x0F 

// Load sequencer mode from flash
#define RFS_SET_LOAD_FL  0x10 

// Store sequencer mode into flash
#define RFS_SET_STORE_FL  0x11 

// set analog gains, DD is 4x2 bits for for channels, each 2 bits encodeds L, M, H, A
#define RFS_SET_GAIN_ANA_SET  0x30 

// automatic analog gains setting, min ADC. Low 2 bits are channels, remaming bits will be multiplied by 16 (1024 max val)
#define RFS_SET_GAIN_ANA_CFG_MIN  0x31 

// automatic analog gains setting, max ADC = min ADC  * mult. Low 2 bits are channels, remaming bits are multiplier.
#define RFS_SET_GAIN_ANA_CFG_MULT  0x32 

// Sets manual bitslicing for XCOR 1-8 (3 LSB bits) to values 1-32 (5 MSB bits)
#define RFS_SET_BITSLICE_LOW  0x33 

// Sets manual bitslicing for XCOR 9-16 (3 LSB bits) to values 1-32 (5 MSB bits)
#define RFS_SET_BITSLICE_HIGH  0x34 

// Uses automatic bitslicing, 0 disables, positive number sets number of SB for lowest product
#define RFS_SET_BITSLICE_AUTO  0x35 

// set routing for ADC channels 1 bits 0-2 are minus, bits 3-6 are plus, bits 7-8 are direct gain
#define RFS_SET_ROUTE_SET1  0x40 

// set routing for ADC channels 2
#define RFS_SET_ROUTE_SET2  0x41 

// set routing for ADC channels 3
#define RFS_SET_ROUTE_SET3  0x42 

// set routing for ADC channels 4
#define RFS_SET_ROUTE_SET4  0x43 

// set ADCs into a special model (0 = disable , 1 = ramp, 2= zeros, 3 = ones)
#define RFS_SET_ADC_SPECIAL  0x44 

// set averaging bit shifts. Lower 4 bits of DD is for Stage1 averager, higher 4 bits is for Stage2 averager. So B9 means 2^9 stage1 averaging and 2^11 stage2 averaging
#define RFS_SET_AVG_SET  0x50 

// set frequency averaging. Valid values are 01, 02, 03, 04. If 03 it averages by 4 ignoring every 4th (presumably PF infected)
#define RFS_SET_AVG_FREQ  0x51 

// set notch averaging, 0 = disabled, 1=x4, 2=x16, 3=x64, 4=x256
#define RFS_SET_AVG_NOTCH  0x52 

// set high priority fraction as a fraction DD/FF, low priorty = 1-high-medium
#define RFS_SET_AVG_SET_HI  0x53 

// set medium priority fraction, low priority is 1-high-medium
#define RFS_SET_AVG_SET_MID  0x54 

// set the output format: 0 - full 32 bits resolution; 1 4+16 bits with update packets
#define RFS_SET_OUTPUT_FORMAT  0x55 

// set the output correlation mask products 0-7 (autocorrelations are 4 LSB)
#define RFS_SET_PRODMASK_LOW  0x56 

// set the output correlation mask products 8-15
#define RFS_SET_PRODMASK_HIGH  0x57 

// Set fraction to reject, 0 to disable rejections
#define RFS_SET_REJ_SET  0x60 

// Number of bad samples to reject a spectrum
#define RFS_SET_REJ_NBAD  0x61 

// Time-resolved spectra starting bin, LSB
#define RFS_SET_TR_START_LSB  0x62 

// Time-resolved spectra stopping bin, LSB
#define RFS_SET_TR_STOP_LSB  0x63 

// Time-resolved spectra stopping MSB (b4-7), starting MSB (b0-3)
#define RFS_SET_TR_ST_MSB  0x64 

// frequency averaging
#define RFS_SET_TR_AVG_SHIFT  0x65 

// set averaging fractions for calibration signal acquisition. Same as 0x50, but note that not all values are valid
#define RFS_SET_CAL_FRAC_SET  0x70 

// set max drift guard in units of 0.1ppm
#define RFS_SET_CAL_MAX_SET  0x71 

// set lock drift guard in units of 0.01ppm
#define RFS_SET_CAL_LOCK_SET  0x72 

// set snr required for lock
#define RFS_SET_CAL_SNR_SET  0x73 

// set starting bin (/(2*4))
#define RFS_SET_CAL_BIN_ST  0x74 

// set end bin (/(2*4))
#define RFS_SET_CAL_BIN_EN  0x75 

// set antenna mask as the lower 4 bits. 0x00001111 = all antennas enabled
#define RFS_SET_CAL_ANT_MASK  0x76 

// enable zoom channel
#define RFS_SET_ZOOM_EN  0x80 

// set zoom 1 input channel
#define RFS_SET_ZOOM_SET1  0x81 

// set zoom 1 spectral channel low bits
#define RFS_SET_ZOOM_SET1_LO  0x82 

// set zoom 1 spectral channel high bits
#define RFS_SET_ZOOM_SET1_HI  0x83 

// set zoom 2 input channel
#define RFS_SET_ZOOM_SET2  0x84 

// set zoom 2 spectral channel# low bits
#define RFS_SET_ZOOM_SET2_LO  0x85 

// set zoom 2 spectral channel# high bits
#define RFS_SET_ZOOM_SET2_HI  0x86 

// enable (DD>0), disable sequencer  (DD=0)
#define RFS_SET_SEQ_EN  0xA0 

// set number of of cycle repetitions, 00 for infinite repetitions
#define RFS_SET_SEQ_REP  0xA1 

// set number of elements in a cycle, restart save counter
#define RFS_SET_SEQ_CYC  0xA2 

// store current configuration, as the next cycle. Store configuration includes settings under 0x30, 0x31, 0x32, 0x33, 0x40, 0x41, 0x50, 0x51, 0x52. DD means the number of integrations under this cycle
#define RFS_SET_SEQ_STO  0xA3 



#endif