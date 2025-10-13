#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include "core_loop.h"
#include "spectrometer_interface.h"
#include "cdi_interface.h"
#include "lusee_appIds.h"
#include "LuSEE_IO.h"
#include "high_prec_avg.h"


void cdi_dispatch_uC (struct cdi_stats* cdi_stats, uint16_t appID, uint32_t length) {
    cdi_stats->cdi_packets_sent++;
    cdi_stats->cdi_bytes_sent += length;
    cdi_dispatch(appID, length);
}

void send_metadata_packet(struct core_state* state) {
    struct meta_data *meta = (struct meta_data *)TLM_BUF;
    wait_for_cdi_ready();
    meta->version = VERSION_ID;
    meta->unique_packet_id = state->unique_packet_id;
    meta->base = state->base;
    cdi_dispatch_uC(&(state->cdi_stats),AppID_MetaData, sizeof(struct meta_data));
    reset_errormasks(state);
}

static int get_shift(struct core_state* state) {
    if (state->base.Navgf == 1)
        return state->base.Navg2_shift;
    if (state->base.Navgf == 2)
        return state->base.Navg2_shift + 1;
    if (state->base.Navgf == 3)
        return state->base.Navg2_shift + 2;
    if (state->base.Navgf == 4)
        return state->base.Navg2_shift + 2;

    // should never get there
    debug_print("E");
    return state->base.Navg2_shift;
}

static void prepare_spectrum_packet(struct core_state* state, uint32_t* data_size, uint32_t* packet_size, char** data_ptr, char** crc_ptr) {
    // compute sizes
    uint8_t format = state->cdi_dispatch.format;
    if (state->cdi_dispatch.format == OUTPUT_32BIT) {
        *data_size = state->cdi_dispatch.Nfreq * sizeof(uint32_t);
    } else if (format == OUTPUT_16BIT_10_PLUS_6 || format == OUTPUT_16BIT_FLOAT1 || format == OUTPUT_16BIT_SHARED_LZ) {
        *data_size = state->cdi_dispatch.Nfreq * sizeof(uint16_t);
    } else if (format == OUTPUT_16BIT_4_TO_5) {
        *data_size = 5 * state->cdi_dispatch.Nfreq * sizeof(uint16_t) / 4;
    }

    char *cdi_ptr = (char*) TLM_BUF;

    // write packet_id
    uint32_t packet_id = state->cdi_dispatch.packet_id;
    memcpy(cdi_ptr, &packet_id, sizeof packet_id);
    cdi_ptr += sizeof packet_id;

    // save pointer to CRC
    *crc_ptr = cdi_ptr;
    cdi_ptr += sizeof(CRC(cdi_ptr, 1));

    *packet_size = (*data_size) + sizeof(packet_id) + sizeof(CRC(cdi_ptr, 1));

    // save pointer to the beginning of actual data
    *data_ptr = cdi_ptr;
}


static void dispatch_data(struct core_state* state) {
    // if we are in tick, we are copyng over TOCK, otherwise TICK !!
    const void *ddr_ptr = spectra_read_buffer(state->tick_tock);
    const uint32_t* ddr_ptr_high = spectra_read_buffer_high(state->tick_tock);
    int offset = state->cdi_dispatch.prod_count * NCHANNELS;

    uint32_t data_size, packet_size;
    char *data_ptr, *crc_ptr;

    prepare_spectrum_packet(state, &data_size, &packet_size, &data_ptr, &crc_ptr);

    const char* const data_start_ptr = data_ptr;

    wait_for_cdi_ready();

    int shift_by = get_shift(state);

    if (state->cdi_dispatch.format == OUTPUT_32BIT) {
        for (int i = 0; i < state->cdi_dispatch.Nfreq; i++) {
            int32_t val = get_averaged_value(ddr_ptr, offset, i, state->cdi_dispatch.Navgf, shift_by, state->base.averaging_mode, ddr_ptr_high);
            memcpy(data_ptr, &val, sizeof val);
            data_ptr += sizeof val;
        }
    } else if (state->cdi_dispatch.format == OUTPUT_16BIT_10_PLUS_6) {
        uint16_t val_out;
        int32_t val_in;

        for (int i = 0; i < state->cdi_dispatch.Nfreq; i++) {
            val_in = get_averaged_value(ddr_ptr, offset, i, state->cdi_dispatch.Navgf, shift_by, state->base.averaging_mode, ddr_ptr_high);
            val_out = encode_10plus6(val_in);
            memcpy(data_ptr, &val_out, sizeof val_out);
            data_ptr += sizeof val_out;
        }
    } else if (state->cdi_dispatch.format == OUTPUT_16BIT_4_TO_5) {
        // buffers for encoding
        int32_t vals_in[4];
        uint16_t vals_out[5];

        for (int i = 0; i <= state->cdi_dispatch.Nfreq; i++) {
            if (i > 0 && i % 4 == 0) {
                // we accumulated 4 values in vals_in;
                // now we compress them into vals_out and copy to CDI buffer
                encode_4_into_5(vals_in, vals_out);
                memcpy(data_ptr, vals_out, sizeof vals_out);
                data_ptr += sizeof vals_out;
            }
            if (i != state->cdi_dispatch.Nfreq) {
                vals_in[i % 4] = get_averaged_value(ddr_ptr, offset, i, state->cdi_dispatch.Navgf, shift_by, state->base.averaging_mode, ddr_ptr_high);
            }
        }
    } else {
        cdi_not_implemented("data format not supported");
    }

    // we don't want to do this ,since the incoming data are still compared
    // against this. Instead, we will zero it in df_transfer
    //memset(ddr_ptr, 0, state.state->cdi_dispatch.Nfreq * sizeof(uint32_t));

    uint32_t crc_value = CRC(data_start_ptr, data_size);
    memcpy(crc_ptr, &crc_value, sizeof crc_value);

    cdi_dispatch_uC(&(state->cdi_stats),state->cdi_dispatch.appId, packet_size);
}

void dispatch_grimm_data(struct core_state *state) {
    // if we are in tick, we are copyng over TOCK, otherwise TICK !!
    const void* ddr_ptr = grimm_spectra_read_buffer(state->tick_tock);
    uint16_t data_size = NSPECTRA * 10 * get_Navg2(state); // we pack 4 int32_t into 5 int16_t

    wait_for_cdi_ready();
    void *cdi_ptr = (char*)TLM_BUF;
    *((uint32_t *)(cdi_ptr)) = state->cdi_dispatch.packet_id;
    cdi_ptr += sizeof(int32_t);
    memcpy(cdi_ptr, ddr_ptr, data_size);
    cdi_dispatch_uC(&(state->cdi_stats),AppID_SpectraGrimm, data_size + sizeof(int32_t));
}

// send NSPECTRA packets
void dispatch_tr_data(struct core_state* state) {
    // TODOS:
    // 1. for loop can incur high cost (cache misses). Just send in contiguous chunks
    // and figure the correct order on the receiving end, if this turns out to be the bottleneck.
    // 2. move memset out of the loop to zero the memory at the end, when all 16 packets have been sent?

    const uint8_t spec_idx = state->cdi_dispatch.tr_count;
    // length of individual chunk we need to copy (corresponds to fixed avg_counter value)
    const uint32_t single_len = get_tr_length(state);

    // nothing to do: return early, do not send header and no data
    if (single_len == 0)
        return;

    const size_t single_size = single_len * sizeof(uint16_t);
    uint16_t Navg2 = get_Navg2(state);

    const size_t data_size = Navg2 * single_size;

    char *cdi_ptr = (char*)TLM_BUF;

    wait_for_cdi_ready();

    // two int32: packet id and crc
    const size_t packet_size = data_size + 2 * sizeof(int32_t);

    memcpy(cdi_ptr, &(state->cdi_dispatch.packet_id), sizeof(state->cdi_dispatch.packet_id));
    cdi_ptr += sizeof(int32_t);

    char *crc_ptr = cdi_ptr;
    cdi_ptr += sizeof(uint32_t);
    // now cdi_ptr points to the beginning of the buffer with actual data
    // save this pointer to compute CRC later
    char* crc_input = cdi_ptr;

    // copy chunks of time resolved spectra
    uint16_t* tr_ptr = (uint16_t*)tr_spectra_read_buffer(state->tick_tock) + spec_idx * single_len;

    for(int i = 0; i < Navg2; ++i) {
        // NB: types of pointers are different, but that is fine, memcpy takes void*
        // and operates byte-wise
        memcpy(cdi_ptr, tr_ptr, single_size);
        cdi_ptr += single_size;
        // zero copied chunk in tick/tock buffer
        tr_ptr += NSPECTRA * single_len;
    }

    // we copied last product, zero TR buffer
    if (spec_idx == NSPECTRA - 1) {
        memset(tr_spectra_read_buffer(state->tick_tock), 0, NSPECTRA * Navg2 *single_size);
    }

    // done copying data, can compute CRC now
    uint32_t crc_value = CRC(crc_input, data_size);
    memcpy(crc_ptr, &crc_value, sizeof crc_value);

    cdi_dispatch_uC(&(state->cdi_stats),state->cdi_dispatch.tr_appId, packet_size);
}


uint32_t get_next_baseAppID(struct core_state* state) {
    // constants from the C standard library implementation of LCG
    update_random_state(state);
    uint8_t rand = state->base.rand_state & 0xFF;
    if (rand <= state->base.hi_frac) {
        return AppID_SpectraHigh;
    } else if (rand <= state->base.hi_frac + state->base.med_frac) {
        return AppID_SpectraMed;
    } else {
        return AppID_SpectraLow;
    }
    // should never be here, really
    return AppID_SpectraLow;
}


uint32_t get_next_tr_baseAppID(struct core_state* state) {
    update_random_state(state);
    uint8_t rand = state->base.rand_state & 0xFF;
    if (rand <= state->base.hi_frac) {
        return AppID_SpectraTRHigh;
    } else if (rand <= state->base.hi_frac + state->base.med_frac) {
        return AppID_SpectraTRMed;
    } else {
        return AppID_SpectraTRLow;
    }
}


void transfer_to_cdi(struct core_state* state) {
    debug_print ("$");
    new_unique_packet_id(state);
    update_time(state);
    send_metadata_packet(state);
    state->cdi_dispatch.prod_count = 0; //
    if (state->base.tr_start<state->base.tr_stop) {
        state->cdi_dispatch.tr_count = 0; // dispatch the TR spectra
    } else {
        state->cdi_dispatch.tr_count = 0xFF; // disable
    }
    if (state->base.grimm_enable) {
        state->cdi_dispatch.grimm_count = 0; // dispatch the grimm spectra
    } else {
        state->cdi_dispatch.grimm_count = 0xFF; // disable
    }
    state->cdi_dispatch.Nfreq = get_Nfreq(state);
    state->cdi_dispatch.Navgf = state->base.Navgf;
    state->cdi_dispatch.appId = get_next_baseAppID(state);
    state->cdi_dispatch.tr_appId = get_next_tr_baseAppID(state);
    state->cdi_dispatch.format = state->base.format;
    state->cdi_dispatch.packet_id = state->unique_packet_id;
    state->timing.cdi_dispatch_counter = tap_counter + state->dispatch_delay;
}

bool delayed_cdi_dispatch_done (struct core_state* state) {
    return (state->cdi_dispatch.prod_count >= NSPECTRA && 
            state->cdi_dispatch.tr_count >=  NSPECTRA   && 
            state->cdi_dispatch.grimm_count >= 1 &&
            state->cdi_dispatch.cal_count >= NCALPACKETS);
}


bool process_delayed_cdi_dispatch (struct core_state* state) {

    // if we are waiting, let's prevent anyone else to send stuff until we are done
    if (state->timing.cdi_dispatch_counter > tap_counter) return true;

    // we always send 16 products + maybe some time resolved + maybe grimm
    // we sent all we had, return to the core loop and let spectra accumulate
    if (delayed_cdi_dispatch_done(state)) {
        // we already sent all spectra, averaged and time resolved, nothing to do
        return false;
    }

#ifdef LN_CORELOOP_TIME_DISPATCH
    timer_start();
#endif

    if (state->cdi_dispatch.prod_count < NSPECTRA) {
        if (state->base.corr_products_mask & (1<<state->cdi_dispatch.prod_count)) {
            debug_print(".");
            dispatch_data(state);
        }
        state->cdi_dispatch.appId++;
        state->cdi_dispatch.prod_count++;

#ifdef LN_CORELOOP_TIME_DISPATCH
        uint32_t elapsed = timer_stop();
        debug_print("disp (s): ");
        debug_print_dec(elapsed);
        debug_print("\r\n");
#endif

    } else if (state->cdi_dispatch.tr_count < NSPECTRA) {
        // actually, this check is redundant
        // if we are here, number_of_time_resolved > 0, and we send it without checks
        debug_print(":");
        dispatch_tr_data(state);
        state->cdi_dispatch.tr_count++;
        state->cdi_dispatch.tr_appId++;

#ifdef LN_CORELOOP_TIME_DISPATCH
        uint32_t elapsed = timer_stop();
        debug_print("disp (tr): ");
        debug_print_dec(elapsed);
        debug_print("\r\n");
#endif
    } else if (state->cdi_dispatch.grimm_count < 1) {
        debug_print("g");
        dispatch_grimm_data(state);
        state->cdi_dispatch.grimm_count++;        
    } else if (state->cdi_dispatch.cal_count < NCALPACKETS) {
        dispatch_calibrator_data(state);
        state->cdi_dispatch.cal_count++;

#ifdef LN_CORELOOP_TIME_DISPATCH
        uint32_t elapsed = timer_stop();
        debug_print("disp (cal): ");
        debug_print_dec(elapsed);
        debug_print("\r\n");
#endif
    }

    state->timing.cdi_dispatch_counter = tap_counter + state->dispatch_delay;
    return true;
}