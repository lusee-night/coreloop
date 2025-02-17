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
    set_calibrator (&(state->cal));
}


void default_state (struct core_state_base *seq)
{
    for (int i = 0; i < NINPUT; i++) {
        seq->gain[i] = GAIN_MED;
        seq->route[i].plus = i;
        seq->route[i].minus = 0xFF;
        seq->gain[i] = GAIN_MED;
        seq->gain_auto_min[i] = (1 << 7);
        seq->gain_auto_mult[i] = (1 << 4);
    }
    seq->Navg1_shift = 14;
    seq->Navg2_shift = 3;
    seq->Navgf = 1;
    for (int i = 0; i < NSPECTRA; i++) seq->bitslice[i] = 0x1F;
    seq->notch = 0;
    seq->hi_frac = 0xFF;
    seq->med_frac = 0x00;
    seq->bitslice_keep_bits = 13;
    seq->format = OUTPUT_32BIT; // OUTPUT_16BIT_UPDATES;
    seq->reject_ratio = 0; // no rejection by default
    seq->reject_maxbad = 0;
    seq->tr_start = 1;
    seq->tr_stop = 0;
}

