#ifndef CORE_LOOP_H
#define CORE_LOOP_H

#include <inttypes.h>
#include "spectrometer_interface.h"


// note that gain auto is missing here, since these are actual spectrometer set gains
enum gain_state {
    GAIN_LOW,
    GAIN_MED,
    GAIN_HIGH,
};


struct route_state {
    uint8_t plus, minus;  // we route "plus" - "minus". if minus is FF, it is ground;
};

struct core_state {
    enum gain_state gain[NINPUT];
    bool  auto_gain[NINPUT];
    struct route_state route[NINPUT];
    uint16_t Navg1, Navg2;
    uint8_t Navg1_shift, Navg2_shift;
};


extern struct core_state state;

void core_loop();

#endif // CORE_LOOP_H