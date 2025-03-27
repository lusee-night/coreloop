#ifndef WATCHDOGS_H
#define WATCHDOGS_H
#include "core_loop.h"
#include "core_loop_errors.h"
#include "LuSEE_IO.h"
#include "cdi_interface.h"
#include "lusee_appIds.h"
#include "spectrometer_interface.h"
#include "lusee_commands.h"
#include <stdint.h>
#include <string.h>

void process_watchdogs (struct core_state* state) {
    
    if (state->watchdog.watchdogs_enabled) {
        feed_uC_watchdog();
        uint8_t tripped = spec_watchdog_tripped();

        if (tripped > 0) {
            struct watchdog_packet* payload = (struct watchdog_packet*)(TLM_BUF);

            new_unique_packet_id(state); // ensures unique_packet_id is incremented
            update_time(state);          // ensure time is fresh
            wait_for_cdi_ready();        // block until CDI buffer is ready

            payload->unique_packet_id = state->unique_packet_id;
            payload->uC_time = state->base.uC_time;
            payload->tripped = tripped;

            cdi_dispatch_uC(&(state->cdi_stats), AppID_Watchdog, sizeof(struct watchdog_packet));

            cmd_soft_reset(0, state);
        }
    }


    int fpga_temp = (TVS_sensors_avg[3]>>7) - 273;
    if (fpga_temp > state->watchdog.FPGA_max_temp) {
        debug_print(" ~~~~ TEMP ALARM ~~~~");
        state->base.errors |= FPGA_TEMP_HIGH;
        if (state->base.spectrometer_enable) RFS_stop(state);
    }   
    for (int i=0; i<4; i++) state->base.TVS_sensors[i] = TVS_sensors_avg[i];
}


//// Main watchdog handling logic
//uint8_t check_and_handle_watchdogs(struct core_state* state) {
//    // Check if watchdogs are enabled
//    if (!state->base.watchdogs_enabled)
//        return 0;
//
//    // Check for a watchdog trip
//    uint8_t tripped = spec_watchdog_tripped();
//
//    if (tripped != 0) {
//        
//        // CDI Buffer?
//        cdi_dispatch(AppID_Watchdog, 1);  // 1 byte payload
//
//        // increment a packet ID counter (if needed)
//        state->unique_packet_id++;
//
//        // Perform a soft reset
//        cmd_soft_reset(0);
//
//        // Exit main loop
//        return 1;
//    }
//
//    return 0;
//}

#endif
