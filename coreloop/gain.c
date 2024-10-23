/* Automatic analog and digital gain functions */
#include "core_loop.h"
#include "LuSEE_IO.h"


void update_spec_gains() {
    for (int8_t i = 0; i < NINPUT; i++) {
        spec_set_gain(i, state.base.actual_gain[i]);
    }
    debug_print("gains changed\r\n");
}

void set_route (uint8_t ch, uint8_t arg_low) {
    state.seq.route[ch].plus = arg_low & 0b111;
    state.seq.route[ch].minus = ((arg_low & 0b111000) >> 3);
    uint8_t gain = ((arg_low & 0b11000000) >> 6);
    spec_set_route(ch, state.seq.route[ch].plus, state.seq.route[ch].minus);
    state.seq.gain[ch] = gain;
    state.base.actual_gain[ch] = gain;
    spec_set_gain(ch, state.base.actual_gain[ch]);
}


bool analog_gain_control() {

    bool gains_changed = false;
    
    for (int i = 0; i < NINPUT; i++) {
        if (state.seq.gain[i] != GAIN_AUTO) continue; // Don't do anything unless AGC is enabled
        int32_t cmax = MAX(state.base.ADC_stat[i].max-0x1FFF, -(state.base.ADC_stat[i].min-0x1FFF));
        if ((state.base.
                ADC_stat[i].invalid_count_max>0) || (state.base.ADC_stat[i].invalid_count_min>0)) cmax = 10000; // blow through.
        //debug_print("AGC: Channel %i max = %i (%i %i) \n", i, cmax, state.gain_auto_max[i], state.seq.gain_auto_min[i]);
        if (cmax > get_gain_auto_max(&state, i)) {
            if (state.base.actual_gain[i] > GAIN_LOW) {
                state.base.actual_gain[i] --;
                state.base.errors |= ((ANALOG_AGC_ACTION_CH1) << i);
                gains_changed = true;
            } else {
                state.base.errors |= (ANALOG_AGC_TOO_HIGH_CH1 <<i);
            }
        } else if (cmax < state.seq.gain_auto_min[i]) {
            if (state.base.actual_gain[i] < GAIN_HIGH) {
                state.base.actual_gain[i] ++;
                state.base.errors |= ((ANALOG_AGC_ACTION_CH1) << i);
                gains_changed = true;
            } else {
                state.base.errors |= (ANALOG_AGC_TOO_LOW_CH1 << i);
            }
        }
    }

    if (gains_changed) update_spec_gains();
    return gains_changed;
}

void process_gain_range() {
    if (spec_get_ADC_stat(state.base.ADC_stat)) {
        if (analog_gain_control()) {
            // gains have changed. wait for settle and trigger. 
            debug_print("\n\rGains changed, resettle\n\r");
            resettle = true;
            resettle_counter = tap_counter+RESETTLE_DELAY;
        } else {
            if (range_adc) {
                range_adc = 0;
                housekeeping_request = 2;
             }
         }
    } else  {
        //if (range_adc) debug_print("not yet \n");
    }

    if ((resettle) & (resettle_counter >= tap_counter)) {
        trigger_ADC_stat();
        resettle = false;
        if (state.base.spectrometer_enable) {
            restart_spectrometer();
        }
    }
}
  
bool bitslice_control() {
    bool bitslice_changed = false;

    for (int i = 0; i < 4; i++) {
        if (state.seq.bitslice[i] != 0xFF) continue; // Don't do anything unless bitslice is auto
        uint8_t keep = 32-leading_zeros_max[i];
        if (keep>(state.seq.bitslice_keep_bits+1)) { 
            // we're keeping more bits than we should (with buffer of 1)
            // slicer should be increased
            state.base.actual_bitslice[i] += (keep-state.seq.bitslice_keep_bits);
            if (state.base.actual_bitslice[i] > 0x1F) {
                state.base.actual_bitslice[i] = 0x1F;
            }
            bitslice_changed = true;
        } else if (keep<(state.seq.bitslice_keep_bits-1)) {
            // we're keeping fewer bits than we should (with buffer of 1)
            // slicer should be decreased
            state.base.actual_bitslice[i] -= MIN(state.seq.bitslice_keep_bits-keep, state.base.actual_bitslice[i]);
            bitslice_changed = true;
        }
    }
    if (bitslice_changed) {

        uint8_t b1 = state.base.actual_bitslice[0];
        uint8_t b2 = state.base.actual_bitslice[1];
        uint8_t b3 = state.base.actual_bitslice[2];
        uint8_t b4 = state.base.actual_bitslice[3];

        if (state.seq.bitslice[4] == 0xFF) state.base.actual_bitslice[4]= (b1+b2)/2-1;
        if (state.seq.bitslice[5] == 0xFF) state.base.actual_bitslice[5]= (b1+b2)/2-1;
        if (state.seq.bitslice[6] == 0xFF) state.base.actual_bitslice[6]= (b1+b3)/2-1;
        if (state.seq.bitslice[7] == 0xFF) state.base.actual_bitslice[7]= (b1+b3)/2-1;
        if (state.seq.bitslice[8] == 0xFF) state.base.actual_bitslice[8]= (b1+b4)/2-1;
        if (state.seq.bitslice[9] == 0xFF) state.base.actual_bitslice[9]= (b1+b4)/2-1;
        if (state.seq.bitslice[10] == 0xFF) state.base.actual_bitslice[10]= (b2+b3)/2-1;
        if (state.seq.bitslice[11] == 0xFF) state.base.actual_bitslice[11]= (b2+b3)/2-1;
        if (state.seq.bitslice[12] == 0xFF) state.base.actual_bitslice[12]= (b2+b4)/2-1;
        if (state.seq.bitslice[13] == 0xFF) state.base.actual_bitslice[13]= (b2+b4)/2-1;
        if (state.seq.bitslice[14] == 0xFF) state.base.actual_bitslice[14]= (b3+b4)/2-1;
        if (state.seq.bitslice[15] == 0xFF) state.base.actual_bitslice[15]= (b3+b4)/2-1;
    }

    return bitslice_changed;
}

