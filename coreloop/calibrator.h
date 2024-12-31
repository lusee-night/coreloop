#ifndef CALIBRATOR_H
#define CALIBRATOR_H
#include <inttypes.h>

// set of modes for the calibrator

// first few modes are for manual control and actually writeablle to the correct register

// spits out calibration data when having a lock
#define CAL_MODE_CAL  0b00
// spits out raw PFB samples
#define CAL_MODE_PFB  0b01
// spits out debug and auxiliary data
#define CAL_MODE_NFO  0b11

// next few modes are for automatic control and are not directly writable to the register
// find SUM bitslice
#define CAL_MODE_BS_SUM 0x10
// find PWR bitslice
#define CAL_MODE_BS_PWR 0x11
// find DFT bitslice
#define CAL_MODE_BS_DFT 0x12
// find FDX/SDX bitslice
#define CAL_MODE_BS_FDX 0x13
//find stage3 bitslice
#define CAL_MODE_BS_S3 0x14

// find SNRon by heuristics
#define CAL_MODE_SNR   0x20
// acquire data automatically 
#define CAL_MODE_RUN   0x30
// run as a blind search mode
#define CAL_MODE_BLIND 0x40
// run as a spectral zoom
#define CAL_MODE_ZOOM  0x50






struct calibrator_state {
    uint8_t mode; // this is the actual model of calibrator. If >4 we are in various auto modes
    uint8_t readout_mode; // this corresponds to the actual register-level readout mode
    uint8_t Navg2, Navg3; // averaging for calibrator
    uint8_t drift_guard, drift_step; // drift guard and step
    uint8_t antenna_mask;
    uint8_t notch_index;
    uint32_t SNRon, SNRoff;
    uint32_t Nsettle, delta_drift_corA, delta_drift_corB; 
    uint16_t pfb_index; // for PFB and spectral zoom mode

    // for saving weights
    uint16_t weight_ndx; // weight index when storing weights
};




#endif
