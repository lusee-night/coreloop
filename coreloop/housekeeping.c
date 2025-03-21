#include "lusee_commands.h"
#include "spectrometer_interface.h"
#include "cdi_interface.h"
#include "lusee_appIds.h"
#include "core_loop.h"
#include <stdlib.h>
#include <stdint.h>
#include "flash_interface.h"
#include "LuSEE_IO.h"
#include <string.h>


void send_hello_packet(struct core_state* state) {
    
    struct startup_hello *payload = (struct startup_hello*) (TLM_BUF);
    new_unique_packet_id(state);
    update_time(state);
    wait_for_cdi_ready();
    payload->SW_version = VERSION_ID;
    payload->FW_Version = spec_get_version(0);;
    payload->FW_ID = spec_get_version(1);
    payload->FW_Date = spec_get_version(2);
    payload->FW_Time = spec_get_version(3);
    payload->unique_packet_id = state->unique_packet_id;
    payload->time_32 = state->base.time_32;
    payload->time_16 = state->base.time_16;
    cdi_dispatch_uC(&(state->cdi_stats),AppID_uC_Start, sizeof(struct startup_hello));
}

bool process_hearbeat(struct core_state* state) {
    if (state->timing.heartbeat_counter >= tap_counter) return false;
    debug_print("H");
    struct heartbeat *payload = (struct heartbeat*) (TLM_BUF);
    wait_for_cdi_ready();
    payload->packet_count = state->heartbeat_packet_count;
    update_time(state);
    payload->time_32 = state->base.time_32;
    payload->time_16 = state->base.time_16;
    payload->TVS_sensors[0] = state->base.TVS_sensors[0];
    payload->TVS_sensors[1] = state->base.TVS_sensors[1];
    payload->TVS_sensors[2] = state->base.TVS_sensors[2];
    payload->TVS_sensors[3] = state->base.TVS_sensors[3];
    payload->cdi_stats = state->cdi_stats;
    payload->errors = state->base.errors;
    
    // loop counters
    state->base.loop_count_min = loop_count_min;
    state->base.loop_count_max = loop_count_max;
    payload->loop_count_min = loop_count_min;
    payload->loop_count_max = loop_count_max;
    loop_count_max = 0;
    loop_count_min = UINT16_MAX;

    payload->magic[0] = 'B';
    payload->magic[1] = 'R';
    payload->magic[2] = 'N';
    payload->magic[3] = 'M';
    payload->magic[4] = 'R';
    payload->magic[5] = 'L';
    cdi_dispatch_uC(&(state->cdi_stats),AppID_uC_Heartbeat, sizeof(struct heartbeat));
    state->timing.heartbeat_counter = tap_counter + HEARTBEAT_DELAY;
    state->heartbeat_packet_count++;
    return true;
}


bool process_housekeeping(struct core_state* state) {
    if (state->housekeeping_request == 0) return false;
    state->housekeeping_request--; // go back to the original one
    struct housekeeping_data_base *base = (struct housekeeping_data_base *)TLM_BUF;
    update_time(state);
    wait_for_cdi_ready();
    base->version = VERSION_ID;
    new_unique_packet_id(state);
    base->unique_packet_id = state->unique_packet_id;
    base->errors = state->base.errors;
    base->housekeeping_type = state->housekeeping_request;

    switch (state->housekeeping_request) {
        case 0:
            debug_print ("[K0]");
            struct housekeeping_data_0 *hk0 = (struct housekeeping_data_0 *)TLM_BUF;
            hk0->core_state = *state;
            cdi_dispatch_uC(&(state->cdi_stats),AppID_uC_Housekeeping, sizeof(struct housekeeping_data_0));
            break;

        case 1:
            debug_print ("[K1]");
            struct housekeeping_data_1 *hk1 = (struct housekeeping_data_1 *)TLM_BUF;
            for (int i=0; i<NINPUT; i++) {
                hk1->ADC_stat[i] = state->base.ADC_stat[i];
                hk1->actual_gain[i] = state->base.actual_gain[i];
            }
            cdi_dispatch_uC(&(state->cdi_stats),AppID_uC_Housekeeping, sizeof(struct housekeeping_data_1));
            break;
    }
    debug_print("E:");
    debug_print_dec(state->base.errors);
    debug_print("\n");

    state->housekeeping_request = 0;
    state->base.errors = 0;
    return true;
}

bool process_eos(struct core_state* state) {
    if (state->request_eos == 0) return false; 
    // now we need to make sure absolutely everything is done
    if (!delayed_cdi_dispatch_done(state)) return false;
    // these two shouldn't really matter, since they get executed in one go
    //  keeping them for sanity (and or future);
    if (state->housekeeping_request) return false;
    if (state->request_waveform) return false;
    struct end_of_sequence *base = (struct end_of_sequence *)TLM_BUF;
    new_unique_packet_id(state);
    base->unique_packet_id = state->unique_packet_id;
    base->eos_arg = state->request_eos;
    cdi_dispatch_uC(&(state->cdi_stats),AppID_End_Of_Sequence, sizeof(struct end_of_sequence));
    state->request_eos = 0;
}