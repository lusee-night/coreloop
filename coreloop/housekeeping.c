#include "lusee_commands.h"
#include "spectrometer_interface.h"
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
    payload->time_seconds = state.base.time_seconds;
    payload->time_subseconds = state.base.time_subseconds;
    cdi_dispatch(AppID_uC_Start, sizeof(struct startup_hello));
}

bool process_hearbeat() {
    if (heartbeat_counter > 0) return false;
    debug_print("H");
    wait_for_cdi_ready();
    char *msg = (char *) TLM_BUF;
    * ((uint32_t*)(TLM_BUF))  =  heartbeat_packet_count;
    heartbeat_packet_count++;
    msg[4] = 'B';
    msg[5] = 'R';
    msg[6] = 'R';
    msg[7] = 'L';

    cdi_dispatch(AppID_uC_Heartbeat, 8);
    heartbeat_counter = HEARTBEAT_DELAY;
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

