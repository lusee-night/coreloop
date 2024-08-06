#include "lusee_commands.h"
#include "spectrometer_interface.h"
#include "lusee_appIds.h"
#include "core_loop.h"
#include <stdlib.h>
#include <stdint.h>
#include "flash_interface.h"
#include "LuSEE_IO.h"
#include <string.h>


void send_metadata_packet() {
    struct meta_data *meta = (struct meta_data *)TLM_BUF;
    wait_for_cdi_ready();
    meta->version = VERSION_ID;
    meta->unique_packet_id = unique_packet_id;
    meta->seq = state.seq;
    meta->base = state.base;
    cdi_dispatch(AppID_MetaData, sizeof(struct meta_data));
    reset_errormasks();
}


void dispatch_32bit_data() {
    // if we are in tick, we are copyng over TOCK, otherwise TICK !!
    int32_t *ddr_ptr = tick_tock ? (int32_t *)(SPEC_TOCK) : (int32_t *)(SPEC_TICK);
    ddr_ptr += state.cdi_dispatch.prod_count * NCHANNELS; //state.Nfreq; // pointer to current block of data.
    int32_t *cdi_ptr = (int32_t *)TLM_BUF;
    int32_t *crc_ptr;

    *cdi_ptr = (int32_t)(state.cdi_dispatch.packet_id);
    cdi_ptr++;
    crc_ptr = cdi_ptr;
    cdi_ptr++;
    uint32_t data_size = state.cdi_dispatch.Nfreq*sizeof(int32_t);
    uint32_t packet_size = data_size+2*sizeof(int32_t);
    wait_for_cdi_ready();
    switch (state.cdi_dispatch.Navgf) {
        case 1:
            memcpy(cdi_ptr, ddr_ptr, state.cdi_dispatch.Nfreq * sizeof(uint32_t));
            break;
        case 2:
            for (int i = 0; i < state.cdi_dispatch.Nfreq; i++) {
                cdi_ptr[i] = ddr_ptr[i*2]<<1 + ddr_ptr[i*2+1]<<1;
            }
            break;
        case 3:
            for (int i = 0; i < state.cdi_dispatch.Nfreq; i++) {
                cdi_ptr[i] = ddr_ptr[i*4]<<2 + ddr_ptr[i*4+1]<<2 + ddr_ptr[i*4+2]<<2; 
            }
            break;
        case 4:
            for (int i = 0; i < state.cdi_dispatch.Nfreq; i++) {
                cdi_ptr[i] = ddr_ptr[i*4]<<2 + ddr_ptr[i*4+1]<<2 + ddr_ptr[i*4+2]<<2 + ddr_ptr[i*4+3]<<2;
            }
            break;
    }

    // we don't want to do this ,since the incoming data are still compared
    // against this. Instead, we will zero it in df_transfer
    //memset(ddr_ptr, 0, state.cdi_dispatch.Nfreq * sizeof(uint32_t));
    *crc_ptr = CRC(cdi_ptr, data_size);
    cdi_dispatch(state.cdi_dispatch.appId, packet_size);
}

void dispatch_16bit_updates_data() {
    cdi_not_implemented("16bit w updates data format");    
}

void dispatch_16bit_float1_data() {
    cdi_not_implemented("16bit w float1 data format");    
}

uint32_t get_next_baseAppID() {
    // constants from the C standard library implementation of LCG
    state.base.rand_state = 1103515245 * state.base.rand_state + 12345;
    uint8_t rand = state.base.rand_state & 0xFF;
    if (rand <= state.seq.hi_frac) {
        return AppID_SpectraHigh;
    } else if (rand <= state.seq.hi_frac + state.seq.med_frac) {
        return AppID_SpectraMed;
    } else {
        return AppID_SpectraLow;
    }
    // should never be here, really
    return AppID_SpectraLow;
}



void transfer_to_cdi () {
    debug_print ("$");

    new_unique_packet_id();
    update_time();
    spec_get_TVS(state.base.TVS_sensors);
    send_metadata_packet();
    state.cdi_dispatch.int_counter = DISPATCH_DELAY; // 10*0.01s ~10 Hz
    state.cdi_dispatch.prod_count = 0; // 
    state.cdi_dispatch.Nfreq = state.Nfreq;
    state.cdi_dispatch.Navgf = state.seq.Navgf;
    state.cdi_dispatch.appId = get_next_baseAppID();
    state.cdi_dispatch.format = state.seq.format;
    state.cdi_dispatch.packet_id = unique_packet_id;

}

bool process_delayed_cdi_dispatch() {
    if (state.cdi_dispatch.int_counter > 0) return false;
    if (state.cdi_dispatch.prod_count > 0x0F)  return false;
    if (state.base.corr_products_mask & (1<<state.cdi_dispatch.prod_count)) {
        debug_print(".");
        switch (state.cdi_dispatch.format) {
            case OUTPUT_32BIT:
                dispatch_32bit_data();
                break;
            case OUTPUT_16BIT_UPDATES:
                dispatch_16bit_updates_data();
                break;
            case OUTPUT_16BIT_FLOAT1:
                dispatch_16bit_float1_data();
                break;
            default:
                cdi_not_implemented("Unsupported output format");
                break;
        }
    }
    state.cdi_dispatch.prod_count++;
    state.cdi_dispatch.appId++;
    state.cdi_dispatch.int_counter = DISPATCH_DELAY;
    return true;
}

