#include "core_loop.h"


bool restart_needed (struct sequencer_state *seq1, struct sequencer_state *seq2 ) {    
    if (seq1->notch != seq2->notch) return true;
    for (int i=0; i<NINPUT; i++) { 
        if (seq1->gain[i] != seq2->gain[i]) return true;
        if (seq1->route[i].plus != seq2->route[i].plus) return true;
        if (seq1->route[i].minus != seq2->route[i].minus) return true;
    }
    for (int i=0; i<NSPECTRA; i++) if (seq1->bitslice[i] != seq2->bitslice[i]) return true;
    return false;
}



void set_spectrometer_to_sequencer()
{
    for (int i = 0; i < NINPUT; i++) {
        //if (state.seq.gain[i]!=GAIN_AUTO)  // not needed and can break initialization
        spec_set_gain(i, state.base.actual_gain[i]);
        spec_set_route(i, state.seq.route[i].plus, state.seq.route[i].minus);
    }
    spec_set_bitslice(state.base.actual_bitslice);
    spec_set_avg1 (state.seq.Navg1_shift);
    spec_notch_enable(state.seq.notch);
    return;
}


void default_seq (struct sequencer_state *seq)
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
    for (int i = 0; i < NSPECTRA; i++) seq->bitslice[i]=0x1F;
    seq->notch = 0;
    seq->hi_frac = 0xFF;
    seq->med_frac = 0x00;
    seq->bitslice_keep_bits=13;
    seq->format =  OUTPUT_32BIT; // OUTPUT_16BIT_UPDATES;
    seq->reject_ratio = 0; // no rejection by default
    seq->reject_maxbad = 0;


}

void advance_sequencer() {

state.base.sequencer_substep--;
if (state.base.sequencer_substep == 0) {
    state.base.sequencer_step = (state.base.sequencer_step+1)%state.program.Nseq;
    if (state.base.sequencer_step == 0) {
        state.base.sequencer_counter++;
        if ((state.program.sequencer_repeat>0) & (state.base.sequencer_counter == state.program.sequencer_repeat)) {
            //debug_print("Sequencer done.\n");
            RFS_stop();
        } else {
            //debug_print("Starting sequencer cycle # %i/%i\n", state.base.sequencer_counter+1, state.base.sequencer_repeat);
        }
    }
    
    state.base.sequencer_substep = state.program.seq_times[state.base.sequencer_step];
    bool restart = restart_needed(&state.seq, &state.program.seq[state.base.sequencer_step]); 
    if (restart) RFS_stop();
    state.seq = state.program.seq[state.base.sequencer_step];
    set_spectrometer_to_sequencer();
    if (restart) RFS_start();
    }      
}



