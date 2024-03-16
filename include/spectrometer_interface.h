#ifndef SPECTROMETER_INTERFACE_H
#define SPECTROMETER__INTERFACE_H

#include <stdbool.h>
#include <stdint.h>

#define NCHANNELS 2048
#define NSPECTRA 16

extern int32_t* DF_BASE_ADDR;
extern bool df_flag;

/******* Internal state of the spectrometer and simulation options *********/

extern uint32_t Navg1;
extern bool add_noise;
extern bool empty_hands_count;  // how many times to return nothing before spectrum on calling new_spectrum_ready;
extern bool spectrometer_enable;

void spectrometer_init();

void spec_set_Navg1(uint32_t Navg1);
void spec_set_spectrometer_enable(bool on);

bool spec_new_spectrum_ready();

static inline void spec_clear_df_flag() { df_flag = false; }
static inline bool spec_df_flag() { return df_flag; }



#endif
