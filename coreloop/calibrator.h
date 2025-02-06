#ifndef CALIBRATOR_H
#define CALIBRATOR_H
#pragma pack(1)
#include <inttypes.h>

// set of modes for the calibrator

// first few modes are for manual control and actually writeablle to the correct register

// spits out calibration data when having a lock
#define CAL_MODE_RAW0  0b00
// spits out raw PFB samples
#define CAL_MODE_RAW1  0b01
// spits out debug and auxiliary data
#define CAL_MODE_RAW3  0b11

// next few modes are for automatic control and are not directly writable to the register
// find SUM bitslice
#define CAL_MODE_BIT_SLICER_SETTLE 0x10
// find SNRon by heuristics
#define CAL_MODE_SNR_SETTLE   0x20
// acquire data automatically 
#define CAL_MODE_RUN   0x30
// run as a blind search mode // to be implemented
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
    uint8_t powertop_slice;
    uint8_t sum1_slice, sum2_slice;
    uint8_t prod1_slice, prod2_slice;
    uint32_t errors;
};


struct calibrator_metadata {
  uint16_t version; 
  uint32_t unique_packet_id;
  uint32_t time_32;
  uint16_t time_16;
  uint16_t have_lock[4];
  struct calibrator_state state;
  int SNR_max, SNR_min;
  int32_t drift [1024];
  uint32_t error_regs [30];
};

struct core_state;

void set_calibrator(struct calibrator_state* cal);
void calibrator_set_SNR(struct calibrator_state* cal);
void calibrator_slice_init(struct calibrator_state* cal);
void calibrator_set_slices(struct calibrator_state* cal);
struct calibrator_metadata* process_cal_mode11(struct core_state* state);


#endif
