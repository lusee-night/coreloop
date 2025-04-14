#ifndef WATCHDOGS_H
#define WATCHDOGS_H
#include "core_loop.h"
#include "core_loop_errors.h"
#include "LuSEE_IO.h"


void process_watchdogs (struct core_state* state) {
    
    int fpga_temp = (TVS_sensors_avg[3]>>7) - 273;
    if (fpga_temp > state->watchdog.FPGA_max_temp) {
        debug_print(" ~~~~ TEMP ALARM ~~~~");
        state->base.errors |= FPGA_TEMP_HIGH;
        if (state->base.spectrometer_enable) RFS_stop(state);
    }   
    for (int i=0; i<4; i++) state->base.TVS_sensors[i] = TVS_sensors_avg[i];
}

#endif
