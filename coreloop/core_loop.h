#ifndef CORE_LOOP_H
#define CORE_LOOP_H

#define VERSION 0.1-DEV
// This 16 bit version ID goes with metadata and startup packets.
// MSB is code version, LSB is metatada version
#define VERSION_ID 0x0101


#include <inttypes.h>
#include "spectrometer_interface.h"
#include "core_loop_errors.h"



// Constants
#define NSEQ_MAX 32

// note that gain auto is missing here, since these are actual spectrometer set gains
enum gain_state{
    GAIN_LOW,
    GAIN_MED,
    GAIN_HIGH,
    GAIN_AUTO};

enum output_format {
    OUTPUT_32BIT,
    OUTPUT_16BIT_UPDATES,
    OUTPUT_16BIT_FLOAT1,
};





struct route_state {
    uint8_t plus, minus;  // we route "plus" - "minus". if minus is FF, it is ground;
};


// sequencer state describes the information needed to set the spectrometer to a given state
struct sequencer_state {
    uint8_t gain [NINPUT]; // this defines the commanded gain state (can be auto)
    uint16_t gain_auto_min[NINPUT];   
    uint16_t gain_auto_mult[NINPUT];
    struct route_state route[NINPUT];
    uint8_t Navg1_shift, Navg2_shift;   // Stage1 (FW) and Stage2 (uC) averaging
    uint8_t notch; // 0 = disable, 1 = x4, 2 = x16, 3=x64, 4=x256
    uint8_t Navgf; // frequency averaging
    uint8_t bitslice[NSPECTRA]; // for spectra 0x1F is all MSB, 0xFF is auto
    uint8_t bitslice_keep_bits; // how many bits to keep for smallest spectra
    uint8_t format;
}__attribute__((packed));

// core state base contains additional information that will be dumped with every metadata packet
struct core_state_base {
    uint32_t time_seconds;
    uint16_t time_subseconds;
    uint16_t TVS_sensors[4]; // temperature and voltage sensors, registers 1.0V, 1.8V, 2.5V and Temp
    uint32_t errors;
    uint8_t actual_gain[NINPUT]; // this defines the actual gain state (can only be low, med, high);
    uint8_t actual_bitslice[NSPECTRA];
    uint16_t spec_overflow;  // mean specta overflow mask
    uint16_t notch_overflow; // notch filter overflow mask
    struct ADC_stat ADC_stat[4];    
    bool spectrometer_enable;
    uint8_t sequencer_counter; // number of total cycles in the sequencer.
    uint8_t sequencer_step; // 0xFF is sequencer is disabled
    uint8_t sequencer_substep; // counting seq_times;
    uint16_t sequencer_repeat; // number of sequencer repeats remaining, 00 for infinite (RFS_SET_SEQ_REP)

}__attribute__((packed));


// core state cointains the seuqencer state and the base state and a number of utility variables
struct core_state {
    struct sequencer_state seq;
    struct core_state_base base;
    // A number be utility values 
    uint16_t Navg1, Navg2;
    uint8_t Navg2_total_shift;
    uint16_t Nfreq; // number of frequency bins after taking into account averaging
    uint16_t gain_auto_max[NINPUT];
    bool sequencer_enabled;
    uint8_t Nseq; // Number of sequencer steps in a cycle (See RFS_SET_SEQ_CYC)
    struct sequencer_state seq_program[NSEQ_MAX]; // sequencer states
    uint16_t seq_times[NSEQ_MAX]; // steps in each sequencer state;
}__attribute__((packed));


struct startup_hello {
    uint16_t version; 
    uint32_t unique_packet_id;
    uint32_t time_seconds;
    uint16_t time_subseconds;
};

// metadata payload, compatible with core_state
struct meta_data {
    uint16_t version; 
    uint32_t unique_packet_id;
    struct sequencer_state seq;
    struct core_state_base base;
} __attribute__((packed));

extern struct core_state state;

void core_loop();

#endif // CORE_LOOP_H