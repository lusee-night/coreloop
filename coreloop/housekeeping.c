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


void send_hello_packet() {
    debug_print ("Sending hello packet.\n\r")
    struct startup_hello *payload = (struct startup_hello*) (TLM_BUF);
    new_unique_packet_id();
    update_time();
    wait_for_cdi_ready();
    payload->SW_version = VERSION_ID;
    payload->FW_Version = spec_get_version(0);;
    payload->FW_ID = spec_get_version(1);
    payload->FW_Date = spec_get_version(2);
    payload->FW_Time = spec_get_version(3);
    payload->unique_packet_id = unique_packet_id;
    payload->time_32 = state.base.time_32;
    payload->time_16 = state.base.time_16;
    cdi_dispatch(AppID_uC_Start, sizeof(struct startup_hello));
}

bool process_hearbeat() {
    if (heartbeat_counter >= tap_counter) return false;
    debug_print("H");
    struct heartbeat *payload = (struct heartbeat*) (TLM_BUF);
    wait_for_cdi_ready();
    payload->packet_count = heartbeat_packet_count;
    update_time();
    payload->time_32 = state.base.time_32;
    payload->time_16 = state.base.time_16;
    payload->magic[0] = 'B';
    payload->magic[1] = 'R';
    payload->magic[2] = 'N';
    payload->magic[3] = 'M';
    payload->magic[4] = 'R';
    payload->magic[5] = 'L';
    cdi_dispatch(AppID_uC_Heartbeat, sizeof(struct heartbeat));
    heartbeat_counter = tap_counter + HEARTBEAT_DELAY;
    heartbeat_packet_count++;
    return true;
}


bool process_housekeeping() {
    if (housekeeping_request == 0) return false;
    housekeeping_request--; // go back to the original one
    struct housekeeping_data_base *base = (struct housekeeping_data_base *)TLM_BUF;
    wait_for_cdi_ready();
    base->version = VERSION_ID;
    base->unique_packet_id = unique_packet_id;
    base->errors = state.base.errors;
    base->housekeeping_type = housekeeping_request;

    switch (housekeeping_request) {
        case 0:
            debug_print ("Sending housekeeping type 0\n\r");
            struct housekeeping_data_0 *hk0 = (struct housekeeping_data_0 *)TLM_BUF;
            hk0->core_state = state;
            cdi_dispatch(AppID_uC_Housekeeping, sizeof(struct housekeeping_data_0));
            break;

        case 1:
            debug_print ("Sending housekeeping type 1\n\r");
            struct housekeeping_data_1 *hk1 = (struct housekeeping_data_1 *)TLM_BUF;
            for (int i=0; i<NINPUT; i++) {
                hk1->ADC_stat[i] = state.base.ADC_stat[i];
                hk1->actual_gain[i] = state.base.actual_gain[i];
            }
            cdi_dispatch(AppID_uC_Housekeeping, sizeof(struct housekeeping_data_1));
            break;

        case 99:
            debug_print ("Sending section break (hk 99)\n\r");
            struct housekeeping_data_99 *hk_99 = (struct housekeeping_data_99 *)TLM_BUF;
            hk_99->section_break = section_break++;
            cdi_dispatch(AppID_uC_Housekeeping, sizeof(struct housekeeping_data_99));
            break;
    }
    debug_print("E:");
    debug_print_dec(state.base.errors);
    debug_print("\n");

    housekeeping_request = 0;
    state.base.errors = 0;
    return true;
}

