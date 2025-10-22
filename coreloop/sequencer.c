#include "core_loop.h"
#include "LuSEE_IO.h"


void set_spectrometer(struct core_state* state)
{
    for (int i = 0; i < NINPUT; i++) {
        spec_set_gain(i, state->base.actual_gain[i]);
        spec_set_route(i, state->base.route[i].plus, state->base.route[i].minus);
    }   
    spec_set_bitslice(state->base.actual_bitslice);
    spec_set_avg1 (state->base.Navg1_shift);
    spec_notch_enable( state->base.notch & 0b111 );
    spec_notch_disable_subtraction( (state->base.notch & 0b10000)>0 );
    spec_enable_notch_detector(     (state->base.notch & 0b100000)>0 );
    set_calibrator (&(state->cal));
}


void default_state (struct core_state_base *base)
{
    for (int i = 0; i < NINPUT; i++) {
        base->gain[i] = GAIN_MED;
        base->route[i].plus = i;
        base->route[i].minus = 0xFF;
        base->gain[i] = GAIN_MED;
        base->gain_auto_min[i] = (1 << 7);
        base->gain_auto_mult[i] = (1 << 4);
    }
    base->Navg1_shift = 14;
    base->Navg2_shift = 3;
    base->Navgf = 1;
    for (int i = 0; i < NSPECTRA; i++) {base->bitslice[i] = 0x1F; base->actual_bitslice[i] = 0x1F;}
    base->notch = 0;
    base->hi_frac = 0xFF;
    base->med_frac = 0x00;
    base->bitslice_keep_bits = 13;
    base->format = OUTPUT_32BIT; // OUTPUT_16BIT_UPDATES;
    base->reject_ratio = 0; // no rejection by default
    base->reject_maxbad = 0;
    base->tr_start = 1;
    base->tr_stop = 0;
    base->grimm_enable = 0;
}

