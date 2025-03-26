/* Automatic analog and digital gain functions */
#include "core_loop.h"
#include "LuSEE_IO.h"


void update_spec_gains(struct core_state* state) {
    for (int8_t i = 0; i < NINPUT; i++) {
        spec_set_gain(i, state->base.actual_gain[i]);
    }
    debug_print("gains changed\r\n");
}

void set_route(struct core_state* state, uint8_t ch, uint8_t arg_low) {
    state->base.route[ch].plus = arg_low & 0b111;
    state->base.route[ch].minus = ((arg_low & 0b111000) >> 3);
    uint8_t gain = ((arg_low & 0b11000000) >> 6);
    spec_set_route(ch, state->base.route[ch].plus, state->base.route[ch].minus);
    state->base.gain[ch] = gain;
    state->base.actual_gain[ch] = gain;
    spec_set_gain(ch, state->base.actual_gain[ch]);
}


bool analog_gain_control(struct core_state* state) {

    bool gains_changed = false;
    
    for (int i = 0; i < NINPUT; i++) {
        if (state->base.gain[i] != GAIN_AUTO) continue; // Don't do anything unless AGC is enabled
        int32_t cmax = MAX(state->base.ADC_stat[i].max-0x1FFF, -(state->base.ADC_stat[i].min-0x1FFF));
        if ((state->base.ADC_stat[i].invalid_count_max > 0) || (state->base.ADC_stat[i].invalid_count_min > 0)) cmax = 10000; // blow through.
        //debug_print("AGC: Channel %i max = %i (%i %i) \n", i, cmax, state->base.gain_auto_max[i], state->base.gain_auto_min[i]);
        if (cmax > get_gain_auto_max(state, i)) {
            if (state->base.actual_gain[i] > GAIN_LOW) {
                state->base.actual_gain[i] --;
                state->base.errors |= ((ANALOG_AGC_ACTION_CH0) << i);
                gains_changed = true;
            } else {
                state->base.errors |= (ANALOG_AGC_TOO_HIGH_CH0 <<i);
            }
        } else if (cmax < state->base.gain_auto_min[i]) {
            if (state->base.actual_gain[i] < GAIN_HIGH) {
                state->base.actual_gain[i] ++;
                state->base.errors |= ((ANALOG_AGC_ACTION_CH0) << i);
                gains_changed = true;
            } else {
                state->base.errors |= (ANALOG_AGC_TOO_LOW_CH0 << i);            
        }
        }   
    }
    if (gains_changed) update_spec_gains(state);
    return gains_changed;
}

void process_gain_range(struct core_state* state) {
    if (spec_get_ADC_stat(state->base.ADC_stat)) {
        if (analog_gain_control(state)) {
            // gains have changed. wait for settle and trigger. 
            debug_print("\n\rGains changed, resettle\n\r");
            state->resettle = true;
            state->timing.resettle_counter = tap_counter+RESETTLE_DELAY;
        } else {
            if (state->range_adc) {
                state->range_adc = 0;
                state->housekeeping_request = 2;
             }
         }
    } else  {
        //if (range_adc) debug_print("not yet \n");
    }

    if ((state->resettle) & (state->timing.resettle_counter >= tap_counter)) {
        trigger_ADC_stat();
        state->resettle = false;
        if (state->base.spectrometer_enable) {
            restart_spectrometer(state);
        }
    }
}
  
bool bitslice_control(struct core_state* state) {
    bool bitslice_changed = false;

    for (int i = 0; i < 4; i++) {
        if (state->base.bitslice[i] != 0xFF) continue; // Don't do anything unless bitslice is auto
        uint8_t keep = 32-state->leading_zeros_max[i];
        if ((keep>(state->base.bitslice_keep_bits+1)) && ( state->base.actual_bitslice[i] != 0x1F)) {
            // we're keeping more bits than we should (with buffer of 1)
            // slicer should be increased
            state->base.actual_bitslice[i] += (keep-state->base.bitslice_keep_bits);
            if (state->base.actual_bitslice[i] > 0x1F) {
                state->base.actual_bitslice[i] = 0x1F;
            }
            bitslice_changed = true;
            state->base.errors |= (DIGITAL_AGC_ACTION_CH0 << i);
        } else if (keep<(state->base.bitslice_keep_bits-1)) {
            // we're keeping fewer bits than we should (with buffer of 1)
            // slicer should be decreased
            state->base.actual_bitslice[i] -= MIN(state->base.bitslice_keep_bits-keep, state->base.actual_bitslice[i]);
            bitslice_changed = true;
            state->base.errors |= (DIGITAL_AGC_ACTION_CH0 << i);
        }
    }
    if (bitslice_changed) {

        
        uint8_t b1 = state->base.actual_bitslice[0];
        uint8_t b2 = state->base.actual_bitslice[1];
        uint8_t b3 = state->base.actual_bitslice[2];
        uint8_t b4 = state->base.actual_bitslice[3];

        if (state->base.bitslice[4] == 0xFF) state->base.actual_bitslice[4]= (b1+b2)/2-1;
        if (state->base.bitslice[5] == 0xFF) state->base.actual_bitslice[5]= (b1+b2)/2-1;
        if (state->base.bitslice[6] == 0xFF) state->base.actual_bitslice[6]= (b1+b3)/2-1;
        if (state->base.bitslice[7] == 0xFF) state->base.actual_bitslice[7]= (b1+b3)/2-1;
        if (state->base.bitslice[8] == 0xFF) state->base.actual_bitslice[8]= (b1+b4)/2-1;
        if (state->base.bitslice[9] == 0xFF) state->base.actual_bitslice[9]= (b1+b4)/2-1;
        if (state->base.bitslice[10] == 0xFF) state->base.actual_bitslice[10]= (b2+b3)/2-1;
        if (state->base.bitslice[11] == 0xFF) state->base.actual_bitslice[11]= (b2+b3)/2-1;
        if (state->base.bitslice[12] == 0xFF) state->base.actual_bitslice[12]= (b2+b4)/2-1;
        if (state->base.bitslice[13] == 0xFF) state->base.actual_bitslice[13]= (b2+b4)/2-1;
        if (state->base.bitslice[14] == 0xFF) state->base.actual_bitslice[14]= (b3+b4)/2-1;
        if (state->base.bitslice[15] == 0xFF) state->base.actual_bitslice[15]= (b3+b4)/2-1;

        debug_print ("\r\nDAGC: ");
        debug_print_dec(b1);
        debug_print(" ");
        debug_print_dec(b2);
        debug_print(" ");
        debug_print_dec(b3);
        debug_print(" ");
        debug_print_dec(b4);
    }

    return bitslice_changed;
}
