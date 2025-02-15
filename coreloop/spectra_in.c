#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "lusee_commands.h"
#include "spectrometer_interface.h"
#include "lusee_appIds.h"
#include "core_loop.h"
#include "high_prec_avg.h"
#include "flash_interface.h"
#include "LuSEE_IO.h"

// this function is not actually used, keeping just in case
//static inline int32_t get_with_zeros(int32_t val, uint8_t *min, uint8_t *max) {
//    int32_t zeros = __builtin_clz(val);
//    *min = MIN(*min, zeros);
//    *max = MAX(*max, zeros);
//    return val;
//}

//
//
//
///* Implementation using 40-bit counters */
//void compute_40bit_averages(int k, int32_t* results) {
//    struct StreamCounters sc;
//    int i, stream_idx;
//
//    /* Initialize all to 0 */
//    for(i = 0; i < NUM_STREAMS; ++i) {
//        sc.low[i] = 0;
//    }
//    for(i = 0; i < 512; ++i) {
//        sc.high[i] = 0;
//    }
//
//    for(i = 0; i < (1 << k); ++i) {
//        for(stream_idx = 0; stream_idx < NUM_STREAMS; ++stream_idx) {
//            int32_t value;
//            int64_t temp_sum;
//
//            generate_stream_value(stream_idx, i, &value);
//
//            /* Combine current counter */
//            temp_sum = ((int64_t)get_high_bits(&sc, stream_idx) << 32) |
//                      (uint32_t)sc.low[stream_idx];
//
//            /* Add new value with precision reduction */
//            temp_sum += (value >> 4);
//
//            /* Store back */
//            sc.low[stream_idx] = (int32_t)temp_sum;
//            set_high_bits(&sc, stream_idx, (int8_t)(temp_sum >> 32));
//        }
//    }
//
//    /* Compute final averages */
//    for(stream_idx = 0; stream_idx < NUM_STREAMS; ++stream_idx) {
//        int64_t final_sum = ((int64_t)get_high_bits(&sc, stream_idx) << 32) |
//                           (uint32_t)sc.low[stream_idx];
//        results[stream_idx] = (int32_t)((final_sum << 4) >> k);
//    }
//}

/// end of tmp

static inline int32_t safe_abs_val(int32_t val)
{
    if (val >= 0) return val;
    if (val == INT32_MIN) return INT32_MAX;
    return -val;
}

bool transfer_time_resolved_from_df(struct core_state* state);

int32_t get_buf();

int64_t get_buf_40();

float get_buf_float();

static inline uint32_t is_bad_int32(const int32_t* df_ptr, const void* ddr_ptr_previous, int total_idx, uint8_t weight_previous, uint8_t reject_ratio)
{
    // in int32 mode we divide by 2^Navg2_shift immediately, before storing to ddr buffer, no need to multiply here
    int32_t prev_val = ((int32_t*)ddr_ptr_previous)[total_idx] / weight_previous;
    int32_t val = *df_ptr;

    if (abs(val-prev_val)>(prev_val/reject_ratio))
        return 1;
    else
        return 0;
}

static inline uint32_t is_bad_float(const int32_t* df_ptr, const void* ddr_ptr_previous, int total_idx, uint8_t weight_previous, uint8_t reject_ratio)
{
    float prev_val = ((float*)ddr_ptr_previous)[total_idx] / weight_previous;
    int32_t val = *df_ptr;

    if (abs(val-prev_val)>(prev_val/reject_ratio))
        return 1;
    else
        return 0;
}

static inline uint32_t is_bad_int40(const int32_t* df_ptr, const void* ddr_ptr_previous, int total_idx, uint8_t weight_previous, uint8_t reject_ratio, uint8_t Navg2_shift)
{
    int64_t prev_val_big = (get_packed_value(ddr_ptr_previous, total_idx) >> Navg2_shift);
    prev_val_big /= weight_previous;
    int32_t previous_val = (int32_t)(prev_val_big & 0xFFFFFFFF);
    int32_t val = *df_ptr;

    if (abs(val-previous_val)>(previous_val/reject_ratio))
        return 1;
    else
        return 0;
}

static inline uint32_t
is_bad(const int32_t* df_ptr, const void* ddr_ptr_previous, int total_idx, uint8_t weight_previous, uint8_t reject_ratio, uint8_t Navg2_shift, uint8_t averaging_mode)
{
    if (averaging_mode == STAGE_2_AVG_INT32) {
        return is_bad_int32(df_ptr, ddr_ptr_previous, total_idx, weight_previous, reject_ratio);
    } else if (averaging_mode == STAGE_2_AVG_INT_40_BITS) {
        return is_bad_int40(df_ptr, ddr_ptr_previous, total_idx, weight_previous, reject_ratio, Navg2_shift);
    } else if (averaging_mode == STAGE_2_AVG_FLOAT) {
        return is_bad_float(df_ptr, ddr_ptr_previous, total_idx, weight_previous, reject_ratio);
    }
}

static inline void
write_spectrum_value(const int32_t value, void* _ddr_ptr, int total_idx, int offset, uint8_t Navg2_shift, uint8_t averaging_mode, int avg_counter)
{
    if (averaging_mode == STAGE_2_AVG_INT32) {

        int32_t* ddr_ptr = (int*)(_ddr_ptr);

        // we divide immediately in this mode
        if (avg_counter == 0)
            ddr_ptr[total_idx] = (value >> Navg2_shift);
        else
            ddr_ptr[total_idx] += (value >> Navg2_shift);

    } else if (averaging_mode == STAGE_2_AVG_FLOAT) {

        float* ddr_ptr = (float*)(_ddr_ptr);

        if (avg_counter == 0)
            ddr_ptr[total_idx] = (float)(value);
        else
            ddr_ptr[total_idx] += (float)(value);

    } else if (averaging_mode == STAGE_2_AVG_INT_40_BITS) {

        struct SpectraIn* ddr_ptr = (struct SpectraIn*)(_ddr_ptr);

        bool is_negative = value < 0;
        uint32_t uvalue = safe_abs_val(value);

        if (avg_counter) {
            uint32_t new_low_bits;
            bool overflow = __builtin_uadd_overflow(ddr_ptr->low[total_idx], uvalue, &new_low_bits);

            if (overflow) {
                ddr_ptr->high[total_idx >> 2] += (1 << ((total_idx %4) * 8));
            }

            ddr_ptr->low[total_idx] = new_low_bits;
        } else {
            // set low bits to value, set high bits to zero in one go for this spectrum
            ddr_ptr->low[total_idx] = uvalue;
            if (total_idx == offset)
                memset(ddr_ptr->high + offset / 4, 0, (NCHANNELS / 4) * sizeof(int32_t));
        }

        // highest of 8 extra bits: sign
        // NB: must be after memset, don't move up
        if (is_negative) {
            ddr_ptr->high[total_idx >> 2] |= (1 << (7 + (total_idx %4) * 8));
        }

    }
}


// return true, if spectra were accepted and copied to TICK/TOCK
// return false, if not accepted
__attribute__((flatten))
bool transfer_from_df(struct core_state* state)
{
    timer_start();
// Want to now transfer all 16 pks worth of data to DDR memory
    int32_t *df_ptr = (int32_t *)SPEC_BUF;
    void* ddr_ptr = spectra_write_buffer(tick_tock);
    const void* ddr_ptr_previous = spectra_read_buffer(tick_tock);
    uint16_t mask = 1;
    bool accept = true;
    //debug_print("Processing spectra...\n\r");
    if ((state->seq.reject_ratio > 0) & (state->base.weight_previous > get_Navg2(state) / 2)) {
        //debug_print("Check for outlier....")
        uint32_t bad = 0;
        for (uint16_t sp = 0; sp < NSPECTRA_AUTO; sp++) {
            int offset = sp * NSPECTRA;
            if (state->base.corr_products_mask & mask) {
                for (int total_idx = offset; total_idx < offset + NCHANNELS; total_idx++) {
                    bad += is_bad(df_ptr, ddr_ptr_previous, total_idx, state->base.weight_previous, state->seq.reject_ratio, state->seq.Navg2_shift, state->seq.averaging_mode);
                    df_ptr++;
                    if (bad > state->seq.reject_maxbad)  {
                        accept = false;
                        // to break the outer loop over sp
                        sp = NSPECTRA_AUTO;
                        break;
                    }
                }
            } else {
                df_ptr += NCHANNELS;
            }
            mask <<= 1;
        }
        // reinitialize the pointers
        df_ptr = (int32_t *)SPEC_BUF;
        mask = 1;
    }

    // do not copy data, if not accepted
    if (accept) {
        state->base.weight_current++;
        for (uint16_t sp = 0; sp < NSPECTRA; sp++) {
            int offset = sp * NCHANNELS;
            //debug_print_dec(sp); debug_print("\n\r");
            if (state->base.corr_products_mask & mask) {
                for (int total_idx = offset; total_idx < offset + NCHANNELS; total_idx++) {
                    write_spectrum_value(df_ptr[total_idx], ddr_ptr, total_idx, offset, state->seq.Navg2_shift, state->seq.averaging_mode, avg_counter);
                }
            }
            mask <<= 1;
        }
    }

    uint32_t elapsed = timer_stop();
    debug_print("avg2: ");
    debug_print_dec(elapsed);
    debug_print("\n ");

    transfer_time_resolved_from_df(state);
    avg_counter++;

    return accept;
}

bool transfer_time_resolved_from_df(struct core_state* state)
{
    if (state->seq.tr_stop <= state->seq.tr_start) {
        return false;
    }

    const int32_t* const df_ptr = (int32_t *)SPEC_BUF;

    uint16_t* const tr_ptr_start = tr_spectra_write_buffer(tick_tock);
    uint16_t* tr_ptr = tr_ptr_start + avg_counter * NSPECTRA * get_tr_length(state);
    if (tr_ptr > tr_ptr_start + TR_SPEC_DATA_SIZE) {
        return false;
    }

    // loop over spectra, skipping those we should ignore according to the mask
    uint16_t mask = 1;
    for (uint16_t sp = 0; sp < NSPECTRA; sp++) {
        if (state->base.corr_products_mask & (mask)) {
            const int32_t* const spectrum_ptr = df_ptr + sp * NCHANNELS;
            for (uint16_t i = state->seq.tr_start; i < state->seq.tr_stop; i += get_tr_avg(state)) {
                int32_t val = 0;
                for (uint16_t j = 0; j < get_tr_avg(state); j++) {
                    val += (spectrum_ptr[i+j] >> state->seq.tr_avg_shift);
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


        if (drop_df) {  // we were asked to drop a frame
            drop_df = false;
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
            bool bit_slice_changed = bitslice_control(state);
            if (bit_slice_changed) {
                restart_spectrometer(state); // Restart the spectrometer if the bit slice has changed; avg_counter will be reset so we don't need to worry about triggering the CDI write
            }

            // Check if we have reached filled up Stage 2 averaging
            // and if so, push things out to CDI
            if (avg_counter == get_Navg2(state)) {
                avg_counter = 0;
                tick_tock = !tick_tock;
                state->base.weight_previous = state->base.weight_current;
                state->base.weight_current = 0;
                // Now one by one, we will loop through the packets placed in DDR Memory
                // For each channel, set the APID, send it to the SRAM
                // Then check to see if this software or the client will control the CDI writes
                transfer_to_cdi(state);
                if (state->sequencer_enabled) advance_sequencer(state);
            }

            if (avg_counter > get_Navg2(state)) debug_print("ERROR: avg_counter exceeded get_Navg2\n");
        }
    }
}
