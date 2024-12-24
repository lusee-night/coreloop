#ifndef WATCHDOGS_H
#define WATCHDOGS_H
#include "core_loop.h"
#include "core_loop_errors.h"
#include "LuSEE_IO.h"


void process_watchdogs (struct core_state* state) {
    spec_get_TVS(state->base.TVS_sensors);
    int fpga_temp = (state->base.TVS_sensors[3]>>4) - 273;
    
    if (fpga_temp > state->watchdog.FPGA_max_temp) {
        debug_print(" ~~~~ TEMP ALARM ~~~~");
        state->base.errors |= FPGA_TEMP_HIGH;
        if (state->base.spectrometer_enable) RFS_stop(state);
    }   
}

#endif
