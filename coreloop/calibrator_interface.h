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
#define SLICER_ERR_PROD1  256
#define SLICER_ERR_PROD2  512


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

// set set weight(0-510)
void calib_set_weight (int index, uint8_t value);

// set notch index to use
void calib_set_notch_index (int index);


#endif