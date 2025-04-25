#include <stdlib.h>
#include <stdint.h>

#include "lusee_commands.h"
#include "spectrometer_interface.h"
#include "lusee_appIds.h"
#include "core_loop.h"
#include "flash_interface.h"
#include "LuSEE_IO.h"
#include "high_prec_avg.h"


static inline int32_t get_with_zeros(int32_t val, uint8_t *min, uint8_t *max) {
    int32_t zeros = __builtin_clz(val);
    *min = MIN(*min, zeros);
    *max = MAX(*max, zeros);
    return val;
}
static inline int32_t safe_abs_val(int32_t val)
{
    if (val >= 0) return val;
    if (val == INT32_MIN) return INT32_MAX;
    return -val;
}

bool transfer_time_resolved_from_df(struct core_state* state);
bool transfer_grimm_from_df(struct core_state* state);

static inline uint32_t is_bad_int32(const int32_t* df_ptr, const void* ddr_ptr_prev, int total_idx, uint8_t weight, uint8_t reject_ratio)
{
    // in int32 mode we divide by 2^Navg2_shift immediately, before storing to ddr buffer, no need to multiply here
    int32_t prev_val = ((int32_t*)ddr_ptr_prev)[total_idx] / weight;
    int32_t val = *df_ptr;

    if (abs(val-prev_val)>(prev_val/reject_ratio))
        return 1;
    else
        return 0;
}

static inline float my_fabsf(float x)
{
    return (x >= 0) ? x : -x;
}

static inline uint32_t is_bad_float(const int32_t* df_ptr, const void* ddr_ptr_prev, int total_idx, uint8_t weight, uint8_t reject_ratio)
{
    float prev_val = ((float*)ddr_ptr_prev)[total_idx] / weight;
    int32_t val = *df_ptr;

    if (my_fabsf(val-prev_val)>(prev_val/reject_ratio))
        return 1;
    else
        return 0;
}

static inline uint32_t is_bad_int40(const int32_t* df_ptr, const void* ddr_ptr_prev, const uint32_t* ddr_ptr_prev_high, int total_idx, uint8_t weight, uint8_t reject_ratio, uint8_t Navg2_shift)
{
    int64_t prev_val_big = (get_packed_value(ddr_ptr_prev, ddr_ptr_prev_high, total_idx) >> Navg2_shift);
    prev_val_big /= weight;
    int32_t previous_val = (int32_t)(prev_val_big & 0xFFFFFFFF);
    int32_t val = *df_ptr;

    if (abs(val-previous_val)>(previous_val/reject_ratio))
        return 1;
    else
        return 0;
}

static inline uint32_t
is_bad(const int32_t* df_ptr, const void* ddr_ptr_prev, const uint32_t* ddr_ptr_prev_high, int total_idx, uint8_t weight, uint8_t reject_ratio, uint8_t Navg2_shift, uint8_t averaging_mode)
{
    if (averaging_mode == AVG_INT32) {
        return is_bad_int32(df_ptr, ddr_ptr_prev, total_idx, weight, reject_ratio);
    } else if (averaging_mode == AVG_INT_40_BITS) {
        return is_bad_int40(df_ptr, ddr_ptr_prev, ddr_ptr_prev_high, total_idx, weight, reject_ratio, Navg2_shift);
    } else if (averaging_mode == AVG_FLOAT) {
        return is_bad_float(df_ptr, ddr_ptr_prev, total_idx, weight, reject_ratio);
    }
}

static inline void
write_spectrum_value(const int32_t value, void* _ddr_ptr, int total_idx, int offset, uint8_t Navg2_shift, uint8_t averaging_mode, int avg_counter, uint32_t* ddr_ptr_high)
{
    if (averaging_mode == AVG_INT32) {

        int32_t* ddr_ptr = (int*)(_ddr_ptr);

        // we divide immediately in this mode
        if (avg_counter == 0)
            ddr_ptr[total_idx] = (value >> Navg2_shift);
        else
            ddr_ptr[total_idx] += (value >> Navg2_shift);

    } else if (averaging_mode == AVG_FLOAT) {

        float* ddr_ptr = (float*)(_ddr_ptr);

        if (avg_counter == 0)
            ddr_ptr[total_idx] = (float)(value);
        else
            ddr_ptr[total_idx] += (float)(value);

    } else if (averaging_mode == AVG_INT_40_BITS) {

        struct SpectraIn* ddr_ptr = (struct SpectraIn*)(_ddr_ptr);

        bool is_negative = value < 0;
        uint32_t uvalue = safe_abs_val(value);

        if (avg_counter) {
            uint32_t new_low_bits;
            bool overflow = __builtin_uadd_overflow(ddr_ptr->low[total_idx], uvalue, &new_low_bits);

            if (overflow) {
                ddr_ptr_high[total_idx >> 2] += (1 << ((total_idx %4) * 8));
            }

            ddr_ptr->low[total_idx] = new_low_bits;
        } else {
            // set low bits to value, set high bits to zero in one go for this spectrum
            ddr_ptr->low[total_idx] = uvalue;
            if (total_idx == offset) {
                memset(ddr_ptr_high + (offset / 4), 0, (NCHANNELS / 4) * sizeof(uint32_t));
            }
        }

        // highest of 8 extra bits: sign
        // NB: must be after memset, don't move up
        if (is_negative) {
            ddr_ptr_high[total_idx >> 2] |= (1 << (7 + (total_idx %4) * 8));
        }

    }
}

bool transfer_time_resolved_from_df(struct core_state* state);

// return true, if spectra were accepted and copied to TICK/TOCK
// return false, if not accepted
bool transfer_from_df(struct core_state* state)
{
// Want to now transfer all 16 pks worth of data to DDR memory
    int32_t *df_ptr = (int32_t *)SPEC_BUF;
    void *ddr_ptr = spectra_write_buffer(state->tick_tock);
    // will only be used if we average using 40 bits, but no need to compute it in every iteration inside write_spectrum_value
    uint32_t* ddr_ptr_high = spectra_write_buffer_high(state->tick_tock);
    const void *ddr_ptr_prev = spectra_read_buffer(state->tick_tock);
    const uint32_t* ddr_ptr_prev_high = spectra_read_buffer_high(state->tick_tock);
    uint16_t mask = 1;
    bool accept = true;
    //debug_print("Processing spectra...\n\r");
    if ((state->base.reject_ratio>0) & (state->base.weight>(get_Navg2(state)/2))) {
        uint32_t bad = 0;

        for (uint16_t sp = 0; sp < NSPECTRA_AUTO; sp++) {

            int offset = sp * NSPECTRA;

            if (state->base.corr_products_mask & mask) {
                for (int total_idx = offset; total_idx < offset + NCHANNELS; total_idx++) {
                    bad += is_bad(df_ptr, ddr_ptr_prev, ddr_ptr_prev_high, total_idx, state->base.weight, state->base.reject_ratio, state->base.Navg2_shift, state->base.averaging_mode);
                    df_ptr++;
                }
            } else {
                df_ptr += NCHANNELS;
            }

            mask <<= 1;
        }

        state->base.num_bad_min_current = MIN(state->base.num_bad_min_current, bad);
        state->base.num_bad_max_current = MAX(state->base.num_bad_max_current, bad);

        if (bad > state->base.reject_maxbad)  {
            accept = false;
            debug_print("X");
        }

        // reinitialize the pointers
        df_ptr = (int32_t *)SPEC_BUF;
        mask = 1;
    }

    if (accept) {
        state->base.weight_current++;
        for (uint16_t sp = 0; sp < NSPECTRA; sp++) {
            int offset = sp * NCHANNELS;
            //debug_print_dec(sp); debug_print("\r\n");
            if (state->base.corr_products_mask & mask) {
                    if (sp < NSPECTRA_AUTO) {
                        state->leading_zeros_min[sp] = 32;
                        state->leading_zeros_max[sp] = 0;

                    for (int total_idx = offset; total_idx < offset + NCHANNELS; total_idx++) {
                        int32_t data = get_with_zeros(df_ptr[total_idx], &state->leading_zeros_min[sp], &state->leading_zeros_max[sp]);
                        write_spectrum_value(data, ddr_ptr, total_idx, offset, state->base.Navg2_shift, state->base.averaging_mode, state->avg_counter, ddr_ptr_high);
                    }
                } else {
                    for (int total_idx = offset; total_idx < offset + NCHANNELS; total_idx++) {
                        write_spectrum_value(df_ptr[total_idx], ddr_ptr, total_idx, offset, state->base.Navg2_shift, state->base.averaging_mode, state->avg_counter, ddr_ptr_high);
                    }
                }
            }
            mask <<= 1;
        }
    }

    transfer_time_resolved_from_df(state);
    transfer_grimm_from_df(state);
    state->avg_counter++;

    return accept;
}

bool transfer_grimm_from_df (struct core_state* state) {
    if (!state->base.grimm_enable) return false;
    for (uint16_t sp = 0; sp < NSPECTRA; sp++) {
        const int32_t* const spectrum_ptr = (int32_t *)SPEC_BUF + sp * NCHANNELS;
        uint32_t vals_in[4];
        vals_in[0] = spectrum_ptr[GRIMM_NDX0];
        vals_in[1] = spectrum_ptr[GRIMM_NDX1];
        vals_in[2] = spectrum_ptr[GRIMM_NDX2];
        vals_in[3] = spectrum_ptr[GRIMM_NDX3];
        uint16_t* const grimm_ptr = grimm_spectra_write_buffer(state->tick_tock) + state->avg_counter * NSPECTRA * 5 * sizeof(uint16_t);
        encode_4_into_5(vals_in, grimm_ptr);
    }
}


bool transfer_time_resolved_from_df(struct core_state* state)
{
    if (state->base.tr_stop <= state->base.tr_start) {
        return false;
    }

    const int32_t* const df_ptr = (int32_t *)SPEC_BUF;

    uint16_t* const tr_ptr_start = tr_spectra_write_buffer(state->tick_tock);
    uint16_t* tr_ptr = tr_ptr_start + state->avg_counter * NSPECTRA * get_tr_length(state);
    if (tr_ptr > tr_ptr_start + TR_SPEC_DATA_SIZE) {
        return false;
    }

    // loop over spectra, skipping those we should ignore according to the mask
    uint16_t mask = 1;
    for (uint16_t sp = 0; sp < NSPECTRA; sp++) {
        if (state->base.corr_products_mask & (mask)) {
            const int32_t* const spectrum_ptr = df_ptr + sp * NCHANNELS;
            for (uint16_t i = state->base.tr_start; i < state->base.tr_stop; i += get_tr_avg(state)) {
                int32_t val = 0;
                for (uint16_t j = 0; j < get_tr_avg(state); j++) {
                    val += (spectrum_ptr[i+j] >> state->base.tr_avg_shift);
                }
                *tr_ptr = encode_10plus6(val);
                tr_ptr++;
            }
        } else {
            // we don't use this product
            // do not write anything to the TR buffer, just advance the pointer
            // buffer contains zeros (memset in the function dispatching TR date)
            tr_ptr += get_tr_length(state);
        }
        mask <<= 1;
    }
    return true;
}

void process_spectrometer(struct core_state* state) {
    // Check if we have a new spectrum packet from the FPGA
    if (spec_new_spectrum_ready()) {
        debug_print ("*");


        if (state->drop_df) {  // we were asked to drop a frame
            state->drop_df = false;
            spec_df_dropped(); // ignore any drooped so far
            spec_clear_df_flag();
        } else {
            if (spec_df_dropped()) state->base.errors |= DF_SPECTRA_DROPPED;
            trigger_ADC_stat();
            transfer_from_df(state);
            uint16_t corr_owf, notch_owf;
            spec_get_digital_overflow(&corr_owf, &notch_owf);
            state->base.spec_overflow |= corr_owf;
            state->base.notch_overflow |= notch_owf;

            spec_clear_df_flag(); // Clear the flag to indicate that we have read the data

            if (state->bitslicer_action_counter<BITSLICER_MAX_ACTION) {
                bool bit_slice_changed = bitslice_control(state);

                if (bit_slice_changed) {
                    (state->bitslicer_action_counter)++;
                    restart_spectrometer(state); // Restart the spectrometer if the bit slice has changed; avg_counter will be reset so we don't need to worry about triggering the CDI write
                 } else {
                    state ->bitslicer_action_counter = 0;
                 }
            } else {
                state->base.errors |= DIGITAL_AGC_STUCK;
            }
            // Check if we have reached filled up Stage 2 averaging
            // and if so, push things out to CDI
            if (state->avg_counter == get_Navg2(state)) {
                state->avg_counter = 0;
                state->tick_tock = !state->tick_tock;
                state->base.weight = state->base.weight_current;
                state->base.num_bad_min = state->base.num_bad_min_current;
                state->base.num_bad_max = state->base.num_bad_max_current;
                // Reset the current values
                state->base.num_bad_min_current = 0xFFFF;
                state->base.num_bad_max_current = 0;
                state->base.weight_current = 0;
                // Now one by one, we will loop through the packets placed in DDR Memory
                // For each channel, set the APID, send it to the SRAM
                // Then check to see if this software or the client will control the CDI writes
                transfer_to_cdi(state);
                if (state->cdi_wait_spectra>0) state->cdi_wait_spectra--;
            }

            if (state->avg_counter > get_Navg2(state)) debug_print("ERROR: avg_counter exceeded get_Navg2\n");
        }
    }
}
