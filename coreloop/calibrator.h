#ifndef CALIBRATOR_H
#define CALIBRATOR_H
#pragma pack(1)
#include <inttypes.h>

// Forward declaration
struct core_state;

// set of modes for the calibrator

// first few modes are for manual control and actually writeablle to the correct register

// spits out calibration data when having a lock
#define CAL_MODE_RAW0  0b00
// spits out raw PFB samples after NNotch
#define CAL_MODE_RAW1  0b01
// spits out raw PFB samples before NNotch
#define CAL_MODE_RAW2  0b10
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


// finetuning of where to adjust slices
#define AUTO_SLICE_SETTLE 0b0001
#define AUTO_SLICE_SNR    0b0010
#define AUTO_SLICE_RUN    0b0100
#define AUTO_SLICE_PROD   0b1000

#define USE_FLOAT_FFT (true)
// happens to be the same as NCHANNELS, but semantically very different
#define NPFB (2048)
#define MAX_SETTLE_COUNT (6)





struct calibrator_state {
    uint8_t mode; // this is the actual model of calibrator. If >4 we are in various auto modes
    uint8_t Navg2, Navg3; // averaging for calibrator
    uint16_t drift_guard, drift_step; // drift guard and step
    uint8_t antenna_mask;
    uint8_t notch_index;
    uint32_t SNRon, SNRoff;
    uint16_t SNR_minratio;
    uint32_t Nsettle, delta_drift_corA, delta_drift_corB;
    uint32_t ddrift_guard, gphase_guard;
    uint16_t pfb_index; // for PFB and spectral zoom mode
    // for saving weights
    uint16_t weight_ndx; // weight index when storing weights
    uint8_t auto_slice;
    uint8_t powertop_slice;
    uint8_t delta_powerbot_slice;
    uint8_t sum1_slice, sum2_slice, fd_slice, sd2_slice;
    uint8_t prod1_slice, prod2_slice;
    uint32_t errors, bitslicer_errors;
    uint8_t zoom_ch1, zoom_ch2;
    bool zoom_diff_1, zoom_diff_2; // if true we difference the two channels
    uint8_t zoom_ch1_minus, zoom_ch2_minus; // if set we difference with channels    
    int32_t zoom_Navg;
    int32_t zoom_avg_idx; // we make this into to be able to use -1 to force it to drop one sample
    int16_t zoom_ndx_range; // range up from pfb_index
    int16_t zoom_ndx_current; // current index (0.. zoom_ndx_range-1)
    //bool use_float_fft; // made into a define
    uint8_t raw11_every, raw11_counter; //  we output raw11 every raw11_every time. 
    uint8_t settle_count; // counts how many settle cycles we have done
};

struct calibrator_stats {
  uint32_t SNR_max[4], SNR_min[4];
  uint32_t ptop_max[4], ptop_min[4];
  uint32_t pbot_max[4], pbot_min[4];
  int32_t FD_max[4], FD_min[4];
  int32_t SD_max[4], SD_min[4];
  uint16_t SD_positive_count[4];
  int32_t lock_count; // keep this 32 bit to avoid alignment issues
};

struct calibrator_error_reg {
  uint32_t cal_phaser_err[2];
  uint32_t averager_err[16];
  uint32_t process_err[8];
  uint32_t stage3_err[4];
  uint32_t check; // fix to 0xFEEDDAD0
};


struct calibrator_metadata {
  uint16_t version;
  uint32_t unique_packet_id;
  uint32_t time_32;
  uint16_t time_16;
  uint16_t have_lock[4];
  uint32_t SNRon, SNRoff;
  uint8_t mode;
  uint8_t powertop_slice;
  uint8_t sum1_slice, sum2_slice, fd_slice, sd2_slice;
  uint8_t prod1_slice, prod2_slice;
  uint32_t errors, bitslicer_errors;
  struct calibrator_stats stats;
  struct calibrator_error_reg error_reg;  
  int16_t drift [128];    
  uint8_t drift_shift;

};


struct saved_calibrator_weights {
  uint32_t in_use;
  uint32_t CRC;
  uint16_t weights[512];
};


struct core_state;

void set_calibrator(struct calibrator_state* cal);
void calibrator_set_SNR(struct calibrator_state* cal);
void calibrator_slice_init(struct calibrator_state* cal);
void calibrator_set_slices(struct calibrator_state* cal);
void process_cal_zoom(struct core_state* state);

#endif
