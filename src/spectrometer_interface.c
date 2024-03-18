#include "spectrometer_interface.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#define _USE_MATH_DEFINES
#include <math.h>


const char* true_spectrum_filename = "data/true_spectrum.dat";
uint32_t true_spectrum[NCHANNELS*NSPECTRA];


/******* Internal state of the spectrometer and simulation options *********/
uint32_t Navg1 = 512;
bool add_noise = true;
bool empty_hands_count = 4;  // how many times to return nothing before spectrum on calling new_spectrum_ready;
bool spectrometer_enable = false;
bool df_flag;
int32_t* DF_BASE_ADDR;
uint8_t channel_gain[NINPUT];


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
    DF_BASE_ADDR = malloc(NCHANNELS*NSPECTRA*sizeof(int32_t));
    printf("Spectrometer init.\n");
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
            DF_BASE_ADDR[i*NCHANNELS+j] = spec;
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
    channel_gain[ch] = gain;
}

// set routing of channel ch to plus - minus
void spec_set_route(uint8_t ch, uint8_t plus, uint8_t minus) {}


void spec_set_avg1 (uint8_t Navg1_shift) {
    Navg1 = (1 << Navg1_shift);
}
