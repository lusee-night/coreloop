#ifndef SPECTROMETER_INTERFACE_H
#define SPECTROMETER_INTERFACE_H

#include <stdbool.h>
#include <stdint.h>




#define NCHANNELS 2048
#define NSPECTRA 16
#define NCALPACKETS 16 
#define NSPECTRA_AUTO 4
#define NINPUT 4
#define UINT14_MAX 16384

// for bit slicing
#define bit_select_A1   0x1F
#define bit_select_A2   0x1F
#define bit_select A3   0x1F
#define bit_select_A4   0x1F
#define bit_select_X12R 0x1F
#define bit_select_X12I 0x1F
#define bit_select_X13R 0x1F
#define bit_select_X13I 0x1F
#define bit_select_X14R 0x1F
#define bit_select_X14I 0x1F
#define bit_select_X23R 0x1F
#define bit_select_X23I 0x1F
#define bit_select_X24R 0x1F
#define bit_select_X24I 0x1F
#define bit_select_X34R 0x1F
#define bit_select_X34I 0x1F

extern uint32_t Navg1;
extern bool add_noise;
extern bool empty_hands_count;  // how many times to return nothing before spectrum on calling new_spectrum_ready;
extern bool spectrometer_enable;
extern bool df_flag;
extern bool adc_trigger;

extern enum ADC_mode {
    ADC_NORMAL_OPS,
    ADC_RAMP
} ADC_mode;

struct ADC_stat {
    int16_t min, max;
    uint32_t valid_count, invalid_count_max, invalid_count_min;
    uint64_t sumv;
    uint64_t sumv2;
}__attribute__((packed));




void spectrometer_init();

void spec_set_Navg1(uint32_t Navg1);

// Enable PFB spectrometer engine
void spec_set_spectrometer_enable(bool on);

// Enable watchdogs
void spec_enable_watchdogs(uint8_t enable);
// Return value if watchdog trips
uint8_t spec_watchdog_tripped(void);
// feed the uC watchdogs, if we don't call this too often it will trip.
void spec_feed_uC_watchdog(void);


// Get various version ID
// subfield s=0 (Version) 1(FW_ID) 2 (FW_Date) 3(FW_Time)
uint32_t spec_get_version(int s);

// RFS_SET_RESET  Reset default configuration (system configuration as after boot)
void spec_set_reset();

// make the CDI interface generate raw waveform (together with delay, set to zero for something sensible)
// returns timestamps for each channel 
void spec_request_waveform(uint8_t ch, int dly, uint64_t* timestamps);

// last_waveform time as implemented in FW 242
uint64_t spec_last_waveform_timestamp();


// set the counter between packets sent by the CDI interface when it needs to chop
void spec_set_fw_cdi_delay(uint32_t delay);

// disable certain channels (by connecting it to ground)
void spec_disable_channel (uint8_t ch);

// set gain of channel ch to gain
void spec_set_gain(uint8_t ch, uint8_t gain);

// get Temmperature and Voltage Telemetry
void spec_get_TVS(uint16_t *TVS);

// set auto and cross-correlation bit-slices for 16 products
//void spec_set_bitslice (uint8_t* slice);
//void compute_index_send(uint32_t in_position, uint32_t inverse_mask, uint32_t reg);

// enable or disable notch filter
// 0 = disable, 1 = x4, 2 = x16, 3=x64, 4=x256
void spec_notch_enable (uint8_t arg);

// disable notch subtraction. This is useful if you want calibrator to run (for which you need the notch engine to run)
// but you don't really want notch- subtracted spectra;
void spec_notch_disable_subtraction(bool disable);

// get overflow bitmasks for wf, 16 correlation channels and their notch counter-parts (MSB)
void spec_get_digital_overflow (uint16_t* corr_owf, uint16_t *notch_owf);


// set routing of channel ch to plus - minus.  plus = 0..3, minus = 0..3 or FF for ground
void spec_set_route(uint8_t ch, uint8_t plus, uint8_t minus);

// set the number of shift bits for Stage 1 averageing (ie. Navg1_shift =10 ->  Navg1 = 1024)
void spec_set_avg1 (uint8_t Navg1_shift);

// set bitislicing for 16 correlation products
void spec_set_bitslice(uint8_t *bitslice);

// fires up the ADC statistic engine
void spec_trigger_ADC_stat(uint16_t Nsamples); 
// If we have ADC results, return true and get ADC level statistics into 4 element array in order to enable automatic gain, etc
bool spec_get_ADC_stat(struct ADC_stat *stat);

// get spacecraft time as per commands RFS_SCM_F1-F3
void spec_get_time(uint32_t *time_sec, uint16_t *time_subsec);

//returns true if a new spectrum is ready (DF flag is set)
bool spec_new_spectrum_ready();

// return true if DF dropped spectra
bool spec_df_dropped(); 

// clears the DF flag
void spec_clear_df_flag();

// ADC test pattern generation

void spec_set_ADC_normal_ops();
void spec_set_ADC_ramp();
void spec_set_enable_digital_func(bool enable);
void spec_set_ADC_all_zeros();
void spec_set_ADC_all_ones();
void spec_set_ADC_toggle_pattern();
void spec_set_ADC_load_custom_pattern();
void spec_set_ADC_custom_pattern();

// read the argument registers from bootloader
uint32_t spec_read_uC_register(uint8_t num);
void spec_write_uC_register(uint8_t num, uint32_t value);

// read writes general register
void spec_reg_write(uint16_t reg, uint32_t value);

#endif
