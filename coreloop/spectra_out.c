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


void send_metadata_packet(struct core_state* state) {
    struct meta_data *meta = (struct meta_data *)TLM_BUF;
    wait_for_cdi_ready();
    meta->version = VERSION_ID;
    meta->unique_packet_id = unique_packet_id;
    meta->seq = state->seq;
    meta->base = state->base;
    cdi_dispatch(AppID_MetaData, sizeof(struct meta_data));
    reset_errormasks(state);
}

// write packet id to the beginning of the buffer
// make crc_ptr to point to the next int32_t after packet_id
// make data_ptr to point to the data buffer (after CRC)
void write_packet_id(uint32_t packet_id, char** data_ptr, char** crc_ptr)
{
    char *cdi_ptr = (char*)TLM_BUF;

    // write packet_id
    memcpy(cdi_ptr, &packet_id, sizeof packet_id);
    cdi_ptr += sizeof packet_id;

    // save pointer to CRC
    *crc_ptr = cdi_ptr;
    cdi_ptr += sizeof(CRC(cdi_ptr, 1));

    // save pointer to the beginning of actual data, to compute its CRC
    *data_ptr = cdi_ptr;
}

void dispatch_32bit_data(struct core_state* state) {
    // if we are in tick, we are copyng over TOCK, otherwise TICK !!
    const int32_t *ddr_ptr = spectra_read_buffer(tick_tock);
    ddr_ptr += state->cdi_dispatch.prod_count * NCHANNELS; //state.Nfreq; // pointer to current block of data.
    int32_t *cdi_ptr = (int32_t *)TLM_BUF;
    int32_t *crc_ptr;

    *cdi_ptr = (int32_t)(state->cdi_dispatch.packet_id);
    cdi_ptr++;
    crc_ptr = cdi_ptr;
    cdi_ptr++;
    uint32_t data_size = state->cdi_dispatch.Nfreq*sizeof(int32_t);
    uint32_t packet_size = data_size+2*sizeof(int32_t);
    wait_for_cdi_ready();
    switch (state->cdi_dispatch.Navgf) {
        case 1:
            memcpy(cdi_ptr, ddr_ptr, state->cdi_dispatch.Nfreq * sizeof(uint32_t));
            break;
        case 2:
            for (int i = 0; i < state->cdi_dispatch.Nfreq; i++) {
                cdi_ptr[i] = (ddr_ptr[i*2]>>1) + (ddr_ptr[i*2+1]>>1);
            }
            break;
        case 3:
            for (int i = 0; i < state->cdi_dispatch.Nfreq; i++) {
                cdi_ptr[i] = (ddr_ptr[i*4]>>2) + (ddr_ptr[i*4+1]>>2) + (ddr_ptr[i*4+2]>>2);
            }
            break;
        case 4:
            for (int i = 0; i < state->cdi_dispatch.Nfreq; i++) {
                cdi_ptr[i] = (ddr_ptr[i*4]>>2) + (ddr_ptr[i*4+1]>>2) + (ddr_ptr[i*4+2]>>2) + (ddr_ptr[i*4+3]>>2);
            }
            break;
    }

    // we don't want to do this ,since the incoming data are still compared
    // against this. Instead, we will zero it in df_transfer
    //memset(ddr_ptr, 0, state.state->cdi_dispatch.Nfreq * sizeof(uint32_t));
    *crc_ptr = CRC(cdi_ptr, data_size);
    cdi_dispatch(state->cdi_dispatch.appId, packet_size);
}

void dispatch_16bit_10_plus_6_data(struct core_state* state) {
    // if we are in tick, we are copyng over TOCK, otherwise TICK !!
    const int32_t *ddr_ptr = spectra_read_buffer(tick_tock);
    ddr_ptr += state->cdi_dispatch.prod_count * NCHANNELS; //state->Nfreq; // pointer to current block of data.

    char* crc_ptr;
    char* data_ptr;
    write_packet_id(state->cdi_dispatch.packet_id, &data_ptr, &crc_ptr);
    char* const data_start_ptr = data_ptr;

    uint32_t data_size = state->cdi_dispatch.Nfreq * sizeof(uint16_t);
    uint32_t packet_size = data_size + 2 * sizeof(int32_t);

    wait_for_cdi_ready();

    uint16_t val_out;
    int32_t val_in;

    for (int i = 0; i < state->cdi_dispatch.Nfreq; i++) {
        if (state->cdi_dispatch.Navgf == 1) {
            val_in = ddr_ptr[i];
        } else if (state->cdi_dispatch.Navgf == 2) {
            val_in = (ddr_ptr[i*2]>>1) + (ddr_ptr[i*2+1]>>1);
        } else if (state->cdi_dispatch.Navgf == 3) {
            val_in = (ddr_ptr[i*4]>>2) + (ddr_ptr[i*4+1]>>2) + (ddr_ptr[i*4+2]>>2);
        } else if (state->cdi_dispatch.Navgf == 4) {
            val_in = (ddr_ptr[i*4]>>2) + (ddr_ptr[i*4+1]>>2) + (ddr_ptr[i*4+2]>>2) + (ddr_ptr[i*4+3]>>2);
        }
        val_out = encode_10plus6(val_in);
        memcpy(data_ptr, &val_out, sizeof val_out);
        data_ptr += sizeof val_out;
    }

    // compute CRC
    uint32_t crc = CRC(data_start_ptr, data_size);
    memcpy(crc_ptr, &crc, sizeof(crc));

    cdi_dispatch(state->cdi_dispatch.appId, packet_size);
}

void dispatch_16bit_updates_data() {
    cdi_not_implemented("16bit w updates data format");
}

void dispatch_16bit_float1_data() {
    cdi_not_implemented("16bit w float1 data format");
}

void dispatch_16bit_shared_lz_data() {
    cdi_not_implemented("16bit shared LZ format");
}

void dispatch_16bit_4_to_5_data(struct core_state* state) {
    // if we are in tick, we are copyng over TOCK, otherwise TICK !!
    const int32_t *ddr_ptr = spectra_read_buffer(tick_tock);
    ddr_ptr += state->cdi_dispatch.prod_count * NCHANNELS;

    char* crc_ptr;
    char* data_ptr;
    write_packet_id(state->cdi_dispatch.packet_id, &data_ptr, &crc_ptr);
    // save pointer to the beginning of actual data, to compute its CRC
    char* const data_start_ptr = data_ptr;

    uint32_t data_size = state->cdi_dispatch.Nfreq * 5 * sizeof(uint16_t) / 4;
    uint32_t packet_size = data_size + 2 * sizeof(int32_t);
    wait_for_cdi_ready();

    // buffers for encoding
    int32_t vals_in[4];
    uint16_t vals_out[5];

    for (int i = 0; i < state->cdi_dispatch.Nfreq; i++) {
        if (i > 0 && i % 4 == 0) {
            // we accumulated 4 values in vals_in;
            // now we compress them into vals_out and copy to CDI buffer
            encode_4_into_5(vals_in, vals_out);
            memcpy(data_ptr, vals_out, sizeof vals_out);
            data_ptr += sizeof vals_out;
        }
        if (state->cdi_dispatch.Navgf == 1) {
            vals_in[i % 4] = ddr_ptr[i];
        } else if (state->cdi_dispatch.Navgf == 2) {
            vals_in[i % 4] = (ddr_ptr[i*2]>>1) + (ddr_ptr[i*2+1]>>1);
        } else if (state->cdi_dispatch.Navgf == 3) {
            vals_in[i % 4] = (ddr_ptr[i*4]>>2) + (ddr_ptr[i*4+1]>>2) + (ddr_ptr[i*4+2]>>2);
        } else if (state->cdi_dispatch.Navgf == 4) {
            vals_in[i] = (ddr_ptr[i*4]>>2) + (ddr_ptr[i*4+1]>>2) + (ddr_ptr[i*4+2]>>2) + (ddr_ptr[i*4+3]>>2);
        }
    }

    // compute CRC
    uint32_t crc = CRC(data_start_ptr, data_size);
    memcpy(crc_ptr, &crc, sizeof(crc));

    cdi_dispatch(state->cdi_dispatch.appId, packet_size);
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
    uint16_t* tr_ptr = (uint16_t*)tr_spectra_read_buffer(tick_tock) + spec_idx * single_len;

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
        memset(tr_spectra_read_buffer(tick_tock), 0, NSPECTRA * Navg2 *single_size);
    }

    // done copying data, can compute CRC now
    uint32_t crc_value = CRC(crc_input, data_size);
    memcpy(crc_ptr, &crc_value, sizeof crc_value);

    cdi_dispatch(state->cdi_dispatch.tr_appId, packet_size);
}


uint32_t get_next_baseAppID(struct core_state* state) {
    // constants from the C standard library implementation of LCG
    update_random_state(state);
    uint8_t rand = state->base.rand_state & 0xFF;
    if (rand <= state->seq.hi_frac) {
        return AppID_SpectraHigh;
    } else if (rand <= state->seq.hi_frac + state->seq.med_frac) {
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
    if (rand <= state->seq.hi_frac) {
        return AppID_SpectraTRHigh;
    } else if (rand <= state->seq.hi_frac + state->seq.med_frac) {
        return AppID_SpectraTRMed;
    } else {
        return AppID_SpectraTRLow;
    }
}


void transfer_to_cdi(struct core_state* state) {
    debug_print ("$");
    new_unique_packet_id();
    update_time(state);
    spec_get_TVS(state->base.TVS_sensors);
    send_metadata_packet(state);
    cdi_dispatch_counter = tap_counter + state->dispatch_delay; // 10*0.01s ~10 Hz
    state->cdi_dispatch.prod_count = 0; //
    if (state->seq.tr_start<state->seq.tr_stop) {
        state->cdi_dispatch.tr_count = 0; // dispatch the TR spectra
    } else {
        state->cdi_dispatch.tr_count = 0xFF; // disable
    }
    state->cdi_dispatch.Nfreq = get_Nfreq(state);
    state->cdi_dispatch.Navgf = state->seq.Navgf;
    state->cdi_dispatch.appId = get_next_baseAppID(state);
    state->cdi_dispatch.tr_appId = get_next_tr_baseAppID(state);
    state->cdi_dispatch.format = state->seq.format;
    state->cdi_dispatch.packet_id = unique_packet_id;
}

bool process_delayed_cdi_dispatch(struct core_state* state) {

    if (cdi_dispatch_counter > tap_counter) return false;
    // we always send 16 products + some time resolved
    // we sent all we had, return to the core loop and let spectra accumulate
    if (state->cdi_dispatch.prod_count >= NSPECTRA && state->cdi_dispatch.tr_count >= NSPECTRA) {
        // we already sent all spectra, averaged and time resolved, nothing to do
        return false;
    }

    if (state->cdi_dispatch.prod_count < NSPECTRA) {
        if (state->base.corr_products_mask & (1<<state->cdi_dispatch.prod_count)) {
            debug_print(".");
            switch (state->cdi_dispatch.format) {
                case OUTPUT_32BIT:
                    dispatch_32bit_data(state);
                    break;
                case OUTPUT_16BIT_UPDATES:
                    dispatch_16bit_updates_data();
                    break;
                case OUTPUT_16BIT_FLOAT1:
                    dispatch_16bit_float1_data();
                    break;
                case OUTPUT_16BIT_10_PLUS_6:
                    dispatch_16bit_10_plus_6_data(state);
                    break;
                case OUTPUT_16BIT_4_TO_5:
                    dispatch_16bit_4_to_5_data(state);
                    break;
                case OUTPUT_16BIT_SHARED_LZ:
                    dispatch_16bit_shared_lz_data();
                    break;
                default:
                    cdi_not_implemented("Unsupported output format");
                    break;
            }
        }
        state->cdi_dispatch.appId++;
        state->cdi_dispatch.prod_count++;
    } else if (state->cdi_dispatch.tr_count < NSPECTRA) {
        // actually, this check is redundant
        // if we are here, number_of_time_resolved > 0, and we send it without checks
        dispatch_tr_data(state);
        state->cdi_dispatch.tr_count++;
        state->cdi_dispatch.tr_appId++;
    }

    cdi_dispatch_counter = tap_counter + state->dispatch_delay;
    return true;
}
