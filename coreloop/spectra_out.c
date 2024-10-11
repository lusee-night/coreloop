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
    const int32_t *ddr_ptr = spectra_read_buffer(tick_tock);
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
                cdi_ptr[i] = (ddr_ptr[i*2]>>1) + (ddr_ptr[i*2+1]>>1);
            }
            break;
        case 3:
            for (int i = 0; i < state.cdi_dispatch.Nfreq; i++) {
                cdi_ptr[i] = (ddr_ptr[i*4]>>2) + (ddr_ptr[i*4+1]>>2) + (ddr_ptr[i*4+2]>>2);
            }
            break;
        case 4:
            for (int i = 0; i < state.cdi_dispatch.Nfreq; i++) {
                cdi_ptr[i] = (ddr_ptr[i*4]>>2) + (ddr_ptr[i*4+1]>>2) + (ddr_ptr[i*4+2]>>2) + (ddr_ptr[i*4+3]>>2);
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

// send NSPECTRA packets
void dispatch_tr_data() {
    // TODOS:
    // 1. for loop can incur high cost (cache misses). Just send in contiguous chunks
    // and figure the correct order on the receiving end, if this turns out to be the bottleneck.
    // 2. move memset out of the loop to zero the memory at the end, when all 16 packets have been sent?

    const uint8_t spec_idx = state.cdi_dispatch.tr_count;
    // length of individual chunk we need to copy (corresponds to fixed avg_counter value)
    const uint32_t single_len = get_tr_length(state);
    const size_t single_size = single_len * sizeof(uint16_t);
    uint16_t Navg2 = get_Navg2(state);

    const size_t data_size = Navg2 * single_size;

    int32_t *cdi_ptr = (int32_t *)TLM_BUF;

    wait_for_cdi_ready();

    // two int32: packet id and crc
    const size_t packet_size = data_size + 2 * sizeof(int32_t);

    *cdi_ptr = (int32_t)(state.cdi_dispatch.packet_id);
    cdi_ptr++;

    uint32_t *crc_ptr = (uint32_t*)cdi_ptr;
    cdi_ptr++;

    uint16_t* tr_ptr = tr_spectra_read_buffer(tick_tock) + spec_idx * single_len;
    for(int i = 0; i < Navg2; ++i) {
        // NB: types of pointers are different, but that is fine, memcpy takes void*
        // and operates byte-wise
        memcpy(cdi_ptr, tr_ptr, single_size);
        // zero copied chunk in tick/tock buffer
        memset(tr_ptr, 0, single_size);
        tr_ptr += NSPECTRA * single_len;
    }
    *crc_ptr = CRC(cdi_ptr, data_size);

    cdi_dispatch(state.cdi_dispatch.tr_appId, packet_size);
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


uint32_t get_next_tr_baseAppID() {
    state.base.rand_state = 1103515245 * state.base.rand_state + 12345;
    uint8_t rand = state.base.rand_state & 0xFF;
    if (rand <= state.seq.hi_frac) {
        return AppID_SpectraTRHigh;
    } else if (rand <= state.seq.hi_frac + state.seq.med_frac) {
        return AppID_SpectraTRMed;
    } else {
        return AppID_SpectraTRLow;
    }
}


void transfer_to_cdi () {
    debug_print ("$");

    new_unique_packet_id();
    update_time();
    spec_get_TVS(state.base.TVS_sensors);
    send_metadata_packet();
    state.cdi_dispatch.int_counter = DISPATCH_DELAY; // 10*0.01s ~10 Hz
    state.cdi_dispatch.prod_count = 0; //
    state.cdi_dispatch.tr_count = 0; //
    state.cdi_dispatch.Nfreq = get_Nfreq(state);
    state.cdi_dispatch.Navgf = state.seq.Navgf;
    state.cdi_dispatch.appId = get_next_baseAppID();
    state.cdi_dispatch.tr_appId = get_next_tr_baseAppID();
    state.cdi_dispatch.format = state.seq.format;
    state.cdi_dispatch.packet_id = unique_packet_id;
}

bool process_delayed_cdi_dispatch() {
    if (state.cdi_dispatch.int_counter > 0) return false;
    // we always send 16 products + some time resolved
    // we sent all we had, return to the core loop and let spectra accumulate
    if (state.cdi_dispatch.prod_count >= NSPECTRA && state.cdi_dispatch.tr_count >= NSPECTRA) {
        // we already sent all spectra, averaged and time resolved, nothing to do
        return false;
    }

    if (state.cdi_dispatch.prod_count < NSPECTRA) {
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
        state.cdi_dispatch.appId++;
        state.cdi_dispatch.prod_count++;
    } else if (state.cdi_dispatch.tr_count < NSPECTRA) {
        // actually, this check is redundant
        // if we are here, number_of_time_resolved > 0, and we send it without checks
        dispatch_tr_data();
        state.cdi_dispatch.tr_count++;
        state.cdi_dispatch.tr_appId++;
    }

    state.cdi_dispatch.int_counter = DISPATCH_DELAY;

    return true;
}