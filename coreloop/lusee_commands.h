
#ifndef LUSEE_SETTINGS_H
#define LUSEE_SETTINGS_H

         
// Top-level CDI commands         
// Master command for uC control
#define RFS_SETTINGS  0x10 

// Special commands for uC control that skip the queue
#define RFS_SPECIAL  0x11 

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

// Wait arg number of mins before processing next CMD (careful with 64 buffer!)
#define RFS_SET_WAIT_MINS  0x0B 

// Wait arg number of mins before processing next CMD (careful with 64 buffer!)
#define RFS_SET_WAIT_HRS  0x0C 

// Debug command (used only in debugging)
#define RFS_SET_DEBUG  0x0D 

// Enable (arg>1) and disable (arg=0) heartbeat.
#define RFS_SET_HEARTBEAT  0x0E 

// prepare for power cut -- mode announcing power cut 5 seconds after issue
#define RFS_SET_TIME_TO_DIE  0x0F 

// Control the underlying FW interpacket delay (1.28ms)
#define RFS_SET_CDI_FW_DLY  0x11 

// Control the delay between spectral packets
#define RFS_SET_CDI_SW_DLY  0x12 

// This writes a register through uC. First command resets value to zero
#define RFS_SET_WR_ADR_LSB  0x13 

// ADR writes adreres, VAL writes value from LSB to MSB
#define RFS_SET_WR_ADR_MSB  0x14 

// Val bits 0-7
#define RFS_SET_WR_VAL_0  0x15 

// Val bits 8-15
#define RFS_SET_WR_VAL_1  0x16 

// Val bits 16-23
#define RFS_SET_WR_VAL_2  0x17 

// Val bits 24-32. This triggers the actual register write
#define RFS_SET_WR_VAL_3  0x18 

// Load sequencer mode from flash
#define RFS_SET_LOAD_FL  0x20 

// Store sequencer mode into flash
#define RFS_SET_STORE_FL  0x21 

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

// Enable / disable the calibrator, bit 0 = enable, bits 1-2 = readout mode
#define RFS_SET_CAL_ENABLE  0x70 

// bits 0-1 Nac, bits 2-5 Nac2
#define RFS_SET_CAL_AVG  0x71 

// Set drift guard in units of 0.1 ppm
#define RFS_SET_CAL_DRIFT_GUARD  0x72 

// Sets drift stepping in units of 0.01ppm
#define RFS_SET_CAL_DRIFT_STEP  0x73 

// bits 0-3 = antenna mask
#define RFS_SET_CAL_ANT_EN  0x74 

// SNR required to get a lock
#define RFS_SET_CAL_SNR_ON  0x75 

// SNR required to drop from a lock
#define RFS_SET_CAL_SNR_OFF  0x76 

// Nsettle
#define RFS_SET_CAL_NSETTLE  0x77 

// Famouse CoRRA settinh
#define RFS_SET_CAL_CORRA  0x78 

// Even more famous CorrB setting
#define RFS_SET_CAL_CORRB  0x79 

// Start setting weights. Set the ndx (0-255)
#define RFS_SET_CAL_WEIGHT_NDX_LO  0x7A 

// Start setting weights. Set the ndx+256
#define RFS_SET_CAL_WEIGHT_NDX_HI  0x7B 

// Sets weigth and advances index
#define RFS_SET_CAL_WEIGHT_VAL  0x7C 

// set calibration mode, including PFB acquisitio debug modes
#define RFS_SET_CAL_MODE  0x7D 

// set PFB NDX (8 LSB bits)
#define RFS_SET_CAL_PFB_NDX_LO  0x7E 

// set PFB NDX (3 MSB bits)
#define RFS_SET_CAL_PFB_NDX_HI  0x7F 

// enable zoom channel
#define RFS_SET_ZOOM_EN  0x90 

// set zoom 1 input channel
#define RFS_SET_ZOOM_SET1  0x91 

// set zoom 1 spectral channel low bits
#define RFS_SET_ZOOM_SET1_LO  0x92 

// set zoom 1 spectral channel high bits
#define RFS_SET_ZOOM_SET1_HI  0x93 

// set zoom 2 input channel
#define RFS_SET_ZOOM_SET2  0x94 

// set zoom 2 spectral channel# low bits
#define RFS_SET_ZOOM_SET2_LO  0x95 

// set zoom 2 spectral channel# high bits
#define RFS_SET_ZOOM_SET2_HI  0x96 

// enable (DD>0), disable sequencer  (DD=0)
#define RFS_SET_SEQ_EN  0xA0 

// set number of of cycle repetitions, 00 for infinite repetitions
#define RFS_SET_SEQ_REP  0xA1 

// set number of elements in a cycle, restart save counter
#define RFS_SET_SEQ_CYC  0xA2 

// store current configuration, as the next cycle. Store configuration includes settings under 0x30, 0x31, 0x32, 0x33, 0x40, 0x41, 0x50, 0x51, 0x52. DD means the number of integrations under this cycle
#define RFS_SET_SEQ_STO  0xA3 



#endif