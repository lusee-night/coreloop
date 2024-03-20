#ifndef CORE_LOOP_H
#define CORE_LOOP_H

#include <inttypes.h>
#include "spectrometer_interface.h"
#include "core_loop_errors.h"

# define NSEQ_MAX 32

// note that gain auto is missing here, since these are actual spectrometer set gains
enum gain_state {
    GAIN_LOW,
    GAIN_MED,
    GAIN_HIGH,
    GAIN_AUTO_LOW,
    GAIN_AUTO_MED,
    GAIN_AUTO_HIGH,
};




struct route_state {
    uint8_t plus, minus;  // we route "plus" - "minus". if minus is FF, it is ground;
};


// sequencer state describes the information needed to set the spectrometer to a given state
struct sequencer_state {
    enum gain_state gain [NINPUT];
    uint8_t gain_auto_min[NINPUT];   
    uint8_t gain_auto_mult[NINPUT];
    struct route_state route[NINPUT];
    uint8_t Navg1_shift, Navg2_shift;   // Stage1 (FW) and Stage2 (uC) averaging
    uint8_t Navgf; // frequency averaging
};

// core state base contains additional information that will be dumped with every metadata packet
struct core_state_base {
    uint32_t errors;
    struct ADC_stat stat[4];    
    bool spectrometer_enable;
    uint8_t sequencer_step; // 0xFF is sequencer is disabled
    uint8_t sequencer_substep; // counting seq_times;
    uint16_t sequencer_repeat; // number of sequencer repeats remaining, 00 for infinite (RFS_SET_SEQ_REP)

};


// core state cointains the seuqencer state and the base state and a number of utility variables
struct core_state {
    struct sequencer_state seq;
    struct core_state_base base;
    // A number be utility values 
    uint16_t Navg1, Navg2;
    uint8_t Navg2_total_shift;
    uint16_t Nfreq; // number of frequency bins after taking into account averaging
    uint8_t gain_auto_max[NINPUT];
    bool sequencer_enabled;
    uint8_t Nseq; // Number of sequencer steps in a cycle (See RFS_SET_SEQ_CYC)
    struct sequencer_state seq_program[NSEQ_MAX]; // sequencer states
    uint16_t seq_times[NSEQ_MAX]; // steps in each sequencer state;
};


// metadata payload, compatible with core_state
struct meta_data {
    struct sequencer_state seq;
    struct core_state_base base;
};

extern struct core_state state;

void core_loop();

#endif // CORE_LOOP_H