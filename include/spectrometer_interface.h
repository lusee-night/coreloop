#ifndef SPECTROMETER_INTERFACE_H
#define SPECTROMETER_INTERFACE_H

#include <stdbool.h>
#include <stdint.h>

#define NCHANNELS 2048
#define NSPECTRA 16
#define NINPUT 4

extern int32_t* DF_BASE_ADDR;
extern bool df_flag;

/******* Internal state of the spectrometer and simulation options *********/

extern uint32_t Navg1;
extern bool add_noise;
extern bool empty_hands_count;  // how many times to return nothing before spectrum on calling new_spectrum_ready;
extern bool spectrometer_enable;

/******************************************************************************/

struct ADC_stat {
    uint32_t invalid_count;
    int32_t min;
    int32_t max;
    uint32_t mean;
    uint64_t var;
};



void spectrometer_init();

void spec_set_Navg1(uint32_t Navg1);
void spec_set_spectrometer_enable(bool on);


// RFS_SET_RESET  Reset default configuration (system configuration as after boot)
void spec_set_reset();

// RFS_SET_STORE Stores current configuration
void spec_store();

// RFS_SET_RECALL  Recalls configuration from previous store 
void spec_recall();

// set gain of channel ch to gain
void spec_set_gain(uint8_t ch, uint8_t gain);

// set routing of channel ch to plus - minus
void spec_set_route(uint8_t ch, uint8_t plus, uint8_t minus);

// set the number of shift bits for Stage 1 averageing (ie. Navg1_shift =10 ->  Navg1 = 1024)
void spec_set_avg1 (uint8_t Navg1_shift);

// Get ADC level statistics into 4 element array in order to enable automatic gain, etc
void spec_get_ADC_stat(struct ADC_stat **stat);


//returns true if a new spectrum is ready (DF flag is set)
bool spec_new_spectrum_ready();
// clears the DF flag
static inline void spec_clear_df_flag() { df_flag = false; }
// returns the value of the DF flag
static inline bool spec_df_flag() { return df_flag; }



#endif
