#include "spectrometer_interface.h"

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <assert.h>

const char* true_spectrum_filename = "data/true_spectrum.dat";
uint32_t true_spectrum[NCHANNELS*NSPECTRA];
uint32_t time_seconds=0;

/******* Internal state of the spectrometer and simulation options *********/
uint32_t Navg1 = 512;
bool add_noise = true;
bool empty_hands_count = 32;  // how many times to return nothing before spectrum on calling new_spectrum_ready;
bool spectrometer_enable = false;
bool df_flag;
bool adc_trigger;
int32_t* SPEC_BUF;
uint8_t channel_gain[NINPUT];

#define N_BOOT_REGISTERS 16
uint32_t boot_registers[N_BOOT_REGISTERS];

// Mapping of channels to cross-correlations
const int ch_ant1[] = {0,1,2,3, 0,0,  0,0,  0,0,  1,1,  1,1, 2, 2};
const int ch_ant2[] = {0,1,2,3, 1,1,  2,2,  3,3,  2,2,  3,3, 3, 3};



void spectrometer_init() {
    FILE* file = fopen(true_spectrum_filename, "r");
    if (file == NULL) {
        printf("Error opening file: %s\n", true_spectrum_filename);
        return;
    }

    for (int i = 0; i < NCHANNELS*NSPECTRA; i++) {
        if (fscanf(file, "%u", &true_spectrum[i]) != 1) {
            printf("Error reading from file: %s\n", true_spectrum_filename);
            fclose(file);
            return;
        }
    }

    fclose(file);
    SPEC_BUF = malloc(NCHANNELS*NSPECTRA*sizeof(int32_t));
    adc_trigger = false;
    printf("Spectrometer init.\n");
    for (int i=0; i<N_BOOT_REGISTERS; i++) {
        boot_registers[i] = 0;
    }
}

uint32_t spec_get_version(int s) {
    switch (s) {
        case 0: return 0x12345678;
        case 1: return 0x87654321;
        case 2: return 0x20240101;
        case 3: return 0x12330000;
        default: return 0;
    }
}

void spec_get_TVS(uint16_t *TVS) {
    TVS[0] = 10;
    TVS[1] = 20;
    TVS[2] = 30;
    TVS[3] = 40;
}


void spec_set_Navg1(uint32_t Navg1) {
    Navg1 = Navg1;
}

void spec_set_spectrometer_enable(bool on) {
    spectrometer_enable = on;
}



double generate_gaussian_variate() {
    double u1 = rand() / (double)RAND_MAX;
    double u2 = rand() / (double)RAND_MAX;
    double z = sqrt(-2 * log(u1)) * cos(2 * M_PI * u2);
    return z;
}


bool spec_new_spectrum_ready() {
    static int count = 0;
    df_flag = false;
    if (!spectrometer_enable) {
        count = 0;
        return false;
    }
    if (count < empty_hands_count) {
        count++;
        return false;
    }
    count = 0;
    df_flag = true;
    for (int i = 0; i < NSPECTRA; i++) {
        for (int j = 0; j < NCHANNELS; j++) {
            int32_t spec = true_spectrum[i*NCHANNELS+j];
            if (add_noise) {
                if (add_noise) {
                    double noise = generate_gaussian_variate();
                    if (i<4) {
                        spec += (int32_t)(2*spec*noise/sqrt(Navg1));
                    } else {
                     int i1 = ch_ant1[i];
                     int i2 = ch_ant2[i];
                     int32_t spec1 = true_spectrum[NCHANNELS*i1+j];
                     int32_t spec2 = true_spectrum[NCHANNELS*i2+j];
                    spec += (int32_t)(sqrt(spec1*spec2+spec*spec)*noise/sqrt(Navg1)/2);
                    }
                }
            }
            SPEC_BUF[i*NCHANNELS+j] = spec;
        }
    }
    return true;
}

void spec_not_implemented() {
    printf("Spectrometer command not implemented.\n");
    exit(1);
}

// RFS_SET_RESET  Reset default configuration (system configuration as after boot)
void spec_set_reset() {spec_not_implemented();};

// RFS_SET_STORE Stores current configuration
void spec_store() {spec_not_implemented();};

// RFS_SET_RECALL  Recalls configuration from previous store 
void spec_recall() {spec_not_implemented();};


// set gain of channel ch to gain
void spec_set_gain(uint8_t ch, uint8_t gain) {
    printf("Setting gain of channel %d to %d\n", ch, gain);
    channel_gain[ch] = gain;
}

// set auto and cross-correlation bit-slices for 16 products
void spec_set_bitslice (uint8_t* slice) {}


// enable or disable notch filter
// 0 = disable, 1 = x4, 2 = x16, 3=x64, 4=x256
void spec_notch_enable (uint8_t arg) {}

// get overflow bitmasks for wf, 16 correlation channels and their notch counter-parts (MSB)
void spec_get_digital_overflow (uint16_t* corr_owf, uint16_t *notch_owf) {
    *corr_owf = 0x0;
    *notch_owf = 0x0;
}


void spec_clear_df_flag() {};


// set routing of channel ch to plus - minus
void spec_set_route(uint8_t ch, uint8_t plus, uint8_t minus) {}


void spec_set_avg1 (uint8_t Navg1_shift) {
    Navg1 = (1 << Navg1_shift);
}



void spec_trigger_ADC_stat(uint16_t Nsamples) {
    adc_trigger = true;
}

bool spec_get_ADC_stat(struct ADC_stat *stat) {
    if (!adc_trigger) return false;
    adc_trigger = false;
    struct ADC_stat ms_med, ms_high, ms_low;
    ms_med.sumv = 0;
    ms_med.sumv2 = 200*200*(1<15);
    ms_med.max = 200*3;
    ms_med.min =-200*3;
    ms_med.invalid_count_max = ms_med.invalid_count_min = 0;
    ms_med.valid_count = 1<<15;

    ms_high.sumv = 0;
    ms_high.sumv2 = 200*200*7*(1<<15);
    ms_high.max = 200*3*7;
    ms_high.min =-200*3*7;
    ms_high.invalid_count_max = ms_high.invalid_count_min = 0;
    ms_high.valid_count = 1<<15;
    
    ms_low.sumv = 0;
    ms_low.sumv2 = 200*200/7*(1<<15);
    ms_low.max = 200*3/7;
    ms_low.min =-200*3/7;
    ms_low.invalid_count_max = ms_low.invalid_count_min = 0;
    ms_low.valid_count = 1<<15;

    for (int i=0; i<NINPUT;i++) {
        assert(channel_gain[i] < 3);
        if (channel_gain[i] ==0) stat[i] = ms_low;
        if (channel_gain[i] ==1) stat[i] = ms_med;
        if (channel_gain[i] ==2) stat[i] = ms_high;
    }
    return true;
}

void spec_request_waveform(uint8_t ch) {}

void spec_disable_channel (uint8_t ch) {}

 void spec_get_time(uint32_t *time_sec, uint16_t *time_subsec){
    *time_sec = time_seconds;
    time_seconds++;
    *time_subsec = 123;
 }

 bool spec_df_dropped() {
     return false;
 }

void spec_set_ADC_normal_ops() {}
void spec_set_ADC_ramp() {}
void spec_set_enable_digital_func(bool enable) {}
void spec_set_ADC_all_zeros() {}
void spec_set_ADC_all_ones() {}
void spec_set_ADC_toggle_pattern() {}
void spec_set_ADC_load_custom_pattern() {}
void spec_set_ADC_custom_pattern() {}

 uint32_t spec_read_uC_register(uint8_t num) {return boot_registers[num];}
 void spec_write_uC_register(uint8_t num, uint32_t value) {boot_registers[num] = value;}