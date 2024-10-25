#include "lusee_commands.h"
#include "spectrometer_interface.h"
#include "lusee_appIds.h"
#include "core_loop.h"
#include <stdlib.h>
#include <stdint.h>
#include "flash_interface.h"
#include "LuSEE_IO.h"


static inline int32_t get_with_zeros(int32_t val, uint8_t *min, uint8_t *max) {
    int32_t zeros = __builtin_clz(val);
    *min = MIN(*min, zeros);
    *max = MAX(*max, zeros);
    return val;
}

bool transfer_time_resolved_from_df();

// return true, if spectra were accepted and copied to TICK/TOCK
// return false, if not accepted
bool transfer_from_df()
{
// Want to now transfer all 16 pks worth of data to DDR memory
    int32_t *df_ptr = (int32_t *)SPEC_BUF;
    int32_t *ddr_ptr = spectra_write_buffer(tick_tock);
    const int32_t *ddr_ptr_previous = spectra_read_buffer(tick_tock);
    uint16_t mask = 1;
    bool accept = true;
    //debug_print("Processing spectra...\n\r");
    if ((state.seq.reject_ratio>0) & (state.base.weight_previous>(get_Navg2(&state)/2))) {
        //debug_print("Check for outlier....")
        uint32_t bad = 0;
        for (uint16_t sp = 0; sp< NSPECTRA_AUTO; sp++) {
            if (state.base.corr_products_mask & (mask)) {
                for (uint16_t i = 0; i < NCHANNELS; i++) {
                    int32_t val = *df_ptr;
                    int32_t previous_val = *ddr_ptr_previous/state.base.weight_previous;            
                    if (abs(val-previous_val)>(previous_val/state.seq.reject_ratio)) {
                        bad++;
                    }
                    df_ptr++;
                    ddr_ptr_previous++;
                }
            } else {df_ptr+=NCHANNELS; ddr_ptr_previous+=NCHANNELS;}
            mask <<= 1;
        }   
        if (bad > state.seq.reject_maxbad)  accept = false;
        // reinitialize the pointers
        df_ptr = (int32_t *)SPEC_BUF;
        mask = 1;
    }
    //debug_print_dec(accept);
    //debug_print("done.\n\r");

    // do not copy data, if not accepted
    if (accept) {
        state.base.weight_current ++;
        for (uint16_t sp = 0; sp < NSPECTRA; sp++) {

            //debug_print_dec(sp); debug_print("\n\r");
            leading_zeros_min[sp] = 32;
            leading_zeros_max[sp] = 0;
            if (state.base.corr_products_mask & (mask)) {
                if (sp < NSPECTRA_AUTO) {
                    
                    for (uint16_t i = 0; i < NCHANNELS; i++) {
                        int32_t data =  (get_with_zeros(*df_ptr, &leading_zeros_min[sp], &leading_zeros_max[sp]) >> state.seq.Navg2_shift);
                        if (avg_counter) {
                            *ddr_ptr += data;
                        } else {
                            *ddr_ptr = data;
                        }
                        df_ptr++;
                        ddr_ptr++;
                    }
                } else {
                    for (uint16_t i = 0; i < NCHANNELS; i++) {
                        int32_t data = (*df_ptr) >> state.seq.Navg2_shift;
                        if (avg_counter) {
                            *ddr_ptr += data;
                        } else {
                            *ddr_ptr = data;
                        }
                        df_ptr++;
                        ddr_ptr++;
                    }
                }
            } else {
                df_ptr+=NCHANNELS; ddr_ptr+=NCHANNELS;
            }
            mask <<= 1;
        }
        transfer_time_resolved_from_df();
        avg_counter++;
    }

    return accept;
}

bool transfer_time_resolved_from_df()
{
    if (state.seq.tr_stop <= state.seq.tr_start) {
        return false;
    }

    const int32_t* const df_ptr = (int32_t *)SPEC_BUF;

    uint16_t* const tr_ptr_start = tr_spectra_write_buffer(tick_tock);
    uint16_t* tr_ptr = tr_ptr_start + avg_counter * NSPECTRA * get_tr_length(&state);
    if (tr_ptr > tr_ptr_start + TR_SPEC_DATA_SIZE) {
        return false;
    }

    // loop over spectra, skipping those we should ignore according to the mask
    uint16_t mask = 1;
    for (uint16_t sp = 0; sp < NSPECTRA; sp++) {
        if (state.base.corr_products_mask & (mask)) {
            const int32_t* const spectrum_ptr = df_ptr + sp * NCHANNELS;
            for (uint16_t i = state.seq.tr_start; i < state.seq.tr_stop; i += get_tr_avg(&state)) {
                int32_t val = 0;
                for (uint16_t j = 0; j < get_tr_avg(&state); j++) {
                    val += (spectrum_ptr[i+j] >> state.seq.tr_avg_shift);
                }
                *tr_ptr = encode_10plus6(val);
                tr_ptr++;
            }
        } else {
            // we don't use this product
            // do not write anything to the TR buffer, just advance the pointer
            // buffer contains zeros (memset in the function dispatching TR date)
            tr_ptr += get_tr_length(&state);
        }
        mask <<= 1;
    }
    return true;
}

void process_spectrometer() {
    // Check if we have a new spectrum packet from the FPGA
    if (spec_new_spectrum_ready()) {
        debug_print ("*");
        

        if (drop_df) {  // we were asked to drop a frame
            drop_df = false;
            spec_df_dropped(); // ignore any drooped so far
            spec_clear_df_flag();
        } else {
            if (spec_df_dropped()) state.base.errors |= DF_SPECTRA_DROPPED;
            trigger_ADC_stat();
            transfer_from_df();
            uint16_t corr_owf, notch_owf;
            spec_get_digital_overflow(&corr_owf, &notch_owf);
            state.base.spec_overflow |= corr_owf;
            state.base.notch_overflow |= notch_owf;

            spec_clear_df_flag(); // Clear the flag to indicate that we have read the data
            bool bit_slice_changed = bitslice_control();
            if (bit_slice_changed) {
                restart_spectrometer(); // Restart the spectrometer if the bit slice has changed; avg_counter will be reset so we don't need to worry about triggering the CDI write
            }

            // Check if we have reached filled up Stage 2 averaging
            // and if so, push things out to CDI
            if (avg_counter == get_Navg2(&state)) {
                avg_counter = 0;                
                tick_tock = !tick_tock;
                state.base.weight_previous = state.base.weight_current;
                state.base.weight_current = 0;
                // Now one by one, we will loop through the packets placed in DDR Memory
                // For each channel, set the APID, send it to the SRAM
                // Then check to see if this software or the client will control the CDI writes
                transfer_to_cdi();
                if (state.sequencer_enabled) advance_sequencer();
            }

            if (avg_counter > get_Navg2(&state)) debug_print("ERROR: avg_counter exceeded get_Navg2\n");
        }
    }
}
