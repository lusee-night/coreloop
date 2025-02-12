#ifndef CALIBRATOR_INTERFACE_H
#define CALIBRATOR_INTERFACE_H

#include <stdint.h>
#include <stdbool.h>

#define CAL_NREGS 498
#define CAL_NWEIGHTS 410
#define CAL_START_WEIGHT 90

// slicer constants (only things we believe we can actually affect). Bit mas for calib_get_slicer_errors;

#define SLICER_ERR_SUM1 1
#define SLICER_ERR_SUM2 2
#define SLICER_ERR_FD   4
#define SLICER_ERR_SD1   8
#define SLICER_ERR_SD2   16
#define SLICER_ERR_SD3   32
#define SLICER_ERR_PTOP  64
#define SLICER_ERR_PBOT  128
#define SLICER_ERR_PR_FDX 256      
#define SLICER_ERR_PR_SDX 512      
#define SLICER_ERR_PR_TOP 1024      
#define SLICER_ERR_PR_BOT 2048 
#define SLICER_ERR_PR_FD  4096    
#define SLICER_ERR_PR_SD  8192
#define SLICER_ERR_PROD1  16384
#define SLICER_ERR_PROD2  32768

#define CAL_ERR_PHASER_0  1
#define CAL_ERR_PHASER_1  2
#define CAL_ERR_PHASER_2  4
#define CAL_ERR_PHASER_3  8
#define CAL_ERR_PHASER_4  16
#define CAL_ERR_PHASER_5  32
#define CAL_ERR_PHASER_6  64
#define CAL_ERR_PHASER_7  128
#define CAL_ERR_AVG_0  256
#define CAL_ERR_AVG_9  512
#define CAL_ERR_AVG_10  1024
#define CAL_ERR_AVG_11 2048
#define CAL_ERR_AVG_12  4096
#define CAL_ERR_AVG_13 8192
#define CAL_ERR_AVG_14 16384
#define CAL_ERR_AVG_15 32768
#define CAL_ERR_PROCESS_0 (1<<16)
#define CAL_ERR_PROCESS_1 (1<<17)
#define CAL_ERR_PROCESS_2 (1<<18)
#define CAL_ERR_PROCESS_3 (1<<19)
#define CAL_ERR_PROCESS_4 (1<<20)
#define CAL_ERR_PROCESS_5 (1<<21)
#define CAL_ERR_PROCESS_24 (1<<22)
#define CAL_ERR_PROCESS_25 (1<<23)
#define CAL_ERR_PROCESS_26 (1<<24) 
#define CAL_ERR_PROCESS_27 (1<<25)
#define CAL_ERR_PROCESS_28 (1<<26)
#define CAL_ERR_PROCESS_29 (1<<27)
#define CAL_ERR_STAG3_12 (1<<28)
#define CAL_ERR_STAG3_13 (1<<29)
#define CAL_ERR_STAG3_14 (1<<30)









// put calibration into a default state
void calib_init();

// Enable calibrator
void calib_enable(bool enable);    

// reset
void cal_reset();

// the hell, slicers
void calib_set_slicers (uint8_t powertop_slice, uint8_t powerbot_slice, uint8_t sum1_slice, \
                        uint8_t sum2_slice, uint8_t sd2_slice, uint8_t prod1_slice, uint8_t prod2_slice);




uint32_t calib_get_slicer_errors();

uint32_t calib_get_errors();



// sets the readout mode
void calib_set_readout_mode (int mode);

// gets the readout mode from the register
int calib_get_readout_mode(); 

// copy all the registers to the buffer
void cal_copy_registers(uint32_t *buffer);

// get and set the PFB index
int calib_get_PFB_index();
void calib_set_PFB_index(int index);

//returns true if a new calibration product is ready
bool cal_new_cal_ready();

// return true if calibrato dropped
bool cal_df_dropped(); 

// clears the DF flag 
void cal_clear_df_flag();

// set averaging number for stage2 and stage3 averages
void calib_set_Navg (int Navg2, int Navg3);

// set drift guard in 0.1 ppm
void calib_set_drift_guard (int val);   


// hold drift fixed
void calib_hold_drift(bool hold);

// set drift guard during lock in 0.001ppm at x16
void calib_set_drift_step (int val); 

// set SNR required to get lock
void calib_set_SNR_lock_on (int val);

// set SNR required to drop lock
void calib_set_SNR_lock_off (int val);

//set Nsettle
void calib_set_Nsettle (int val);

// set delta_drift_corA and B
void calib_set_delta_drift_corA (int val);
void calib_set_delta_drift_corB (int val);

// set antennas mask
void calib_antenna_mask (int mask);

// setting weights to zero
void calib_zero_weights(); 

// setting weights to unit
void calib_unit_weights();

// set set weight(0-510)
void calib_set_weight (int index, uint8_t value);

// set notch index to use
void calib_set_notch_index (int index);


#endif