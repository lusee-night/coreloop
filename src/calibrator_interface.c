#include "calibrator_interface.h"
#include "LuSEE_IO.h"
#include <stdlib.h>


void calib_init() {
    // Placeholder implementation
    CAL_DF = malloc(128*1024);
}

void calib_enable(bool enable) {
    // Placeholder implementation
}   

void calib_set_readout_mode(int mode) {
    // Placeholder implementation
}

void cal_copy_registers(uint32_t *buffer){
    // Placeholder implementation
}

int calib_get_PFB_index() {
    // Placeholder implementation
    return 0;
}

void calib_set_PFB_index(int index) {
    // Placeholder implementation
}

//returns true if a new calibration product is ready
void cal_new_cal_ready(bool* modes) {}

// assuming a certain mode is ready above, transfer it over
void cal_transfer_data(int mode) {}

bool cal_df_dropped() {
    // Placeholder implementation
    return false;
}

void cal_clear_df_flag() {
    // Placeholder implementation
}

void calib_set_Navg(int Navg2, int Navg3) {
    // Placeholder implementation
}

void calib_set_drift_guard(int val) {
    // Placeholder implementation
}

void calib_set_drift_step(int val) {
    // Placeholder implementation
}

void calib_set_SNR_lock_on(int val) {
    // Placeholder implementation
}

void calib_set_SNR_lock_off(int val) {
    // Placeholder implementation
}

void calib_set_Nsettle(int val) {
    // Placeholder implementation
}

void calib_set_delta_drift_corA(int val) {
    // Placeholder implementation
}

void calib_set_delta_drift_corB(int val) {
    // Placeholder implementation
}

void calib_antenna_mask(int mask) {
    // Placeholder implementation
}

void calib_zero_weights() {
    // Placeholder implementation
}

void calib_set_weight(int index, uint16_t value) {
    // Placeholder implementation
}

void calib_hold_drift(bool hold) {
}

void calib_set_notch_index(int index) {
    // Placeholder implementation
}



void cal_reset() {};

void calib_set_slicers (uint8_t powertop_slice, uint8_t powerbot_slice, uint8_t sum1_slice, \
                        uint8_t sum2_slice, uint8_t sd2_slice, uint8_t prod1_slice, uint8_t prod2_slice) {}




uint32_t calib_get_slicer_errors() {return 0;}

uint32_t calib_get_errors() {return 0;}

int calib_get_readout_mode() {}

void calib_set_ddrift_max(int val) {}

void calib_set_gphase_max(int val) {}