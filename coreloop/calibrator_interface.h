#ifndef CALIBRATOR_INTERFACE_H
#define CALIBRATOR_INTERFACE_H

#include <stdint.h>
#include <stdbool.h>

#define NCALREGS 497

// put calibration into a default state
void calib_init();

// Enable calibrator
void calib_enable(bool enable);    

// sets the readout mode
void calib_set_readout_mode (int mode);

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

// set drift step in 0.01 ppm
void calib_set_drift_step (int val);   

// set drift guard during lock in 0.001ppm
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




#endif