#include <string.h> // Add include for memcpy
#include <stdlib.h>

#include "global.h"
#include "core_loop.h"
#include "spectrometer_interface.h"
#include "cdi_interface.h"
#include "lusee_settings.h"


/***************** GLOBAL STATE ******************/

struct core_state state;
uint16_t avg_counter = 0;

/**************************************************/


void cdi_not_implemented(const char *msg)
{
    debug_print("CDI command not implemented: %s\n",msg);
    exit(1);
    return;
}


static inline uint8_t gain_to_spec_gain(enum gain_state gain)
{
    if (gain<GAIN_AUTO_LOW) return gain; else return gain-GAIN_AUTO_LOW;
}

void set_spectrometer_to_sequencer()
{
    for (int i = 0; i < NINPUT; i++) {
        spec_set_gain(i, state.seq.gain[i]);
        spec_set_route(i, state.seq.route[i].plus, state.seq.route[i].minus);
    }
    spec_set_avg1 (state.seq.Navg1_shift);
    return;
}


void default_seq (struct sequencer_state *seq)
{
    for (int i = 0; i < NINPUT; i++) {
        seq->gain[i] = GAIN_MED;
        seq->route[i].plus = i;
        seq->route[i].minus = 0xFF;
    }
    seq->Navg1_shift = 11;
    seq->Navg2_shift = 9;
    seq->Navgf = 1;
}

void fill_derived() {
    state.Navg1 = 1 << state.seq.Navg1_shift;
    state.Navg2 = 1 << state.seq.Navg2_shift;
    // total shift takes into account frequency averaging;
    state.Navg2_total_shift = state.seq.Navg2_shift;
    state.Nfreq = NCHANNELS; 
    if (state.seq.Navgf == 2 ) { state.Navg2_total_shift += 1; state.Nfreq = NCHANNELS/2; }
    if ((state.seq.Navgf == 3 ) || (state.seq.Navgf == 4)) { state.Navg2_total_shift += 2; state.Nfreq = NCHANNELS/4;} 
    
}




void core_init_state(){
    default_seq (&state.seq);
    state.base.errors = 0;
    state.base.spectrometer_enable = false;
    spec_set_spectrometer_enable(false);
    state.base.sequencer_step = 0xFF;
    state.sequencer_enabled = false;
    state.Nseq = 0;
    fill_derived();
    set_spectrometer_to_sequencer();
}

void RFS_stop() {
    state.base.spectrometer_enable = false;
    spec_set_spectrometer_enable(false);
}

void RFS_start() {
    state.base.spectrometer_enable = true;
    avg_counter = 0;
    int32_t *ddr_ptr = (int32_t *)DDR3_BASE_ADDR;
    memset(ddr_ptr, 0, NCHANNELS * sizeof(uint32_t));
    if (state.sequencer_enabled) {
        state.base.sequencer_counter = 0;
        state.base.sequencer_step = 0;
        state.base.sequencer_substep = state.seq_times[0];
        state.seq = state.seq_program[0];
    }
    fill_derived();
    set_spectrometer_to_sequencer();
    spec_set_spectrometer_enable(true);

}

void cdi_process_command(uint8_t cmd, uint8_t arg_high, uint8_t arg_low)
{
    // Do something with the command
    debug_print("Received command: %x %x %x\n", cmd, arg_high, arg_low);
    if (cmd==RFS_Settings)  {
        switch (arg_high) {
            case RFS_SET_START:
                RFS_start();
                return;
            case RFS_SET_STOP:
                RFS_stop();
                return;                
            case RFS_SET_RESET:
                spec_set_reset();
                core_init_state();
                return;
            case RFS_SET_STORE:
                spec_store();
                return;
            case RFS_SET_RECALL:
                spec_recall();
                return;
            case RFS_SET_HK_REQ:
                cdi_not_implemented("RFS_SET_HK_REQ");
                return;
            case RFS_SET_TIME_TO_DIE:
                // this is handled in the main loop
                return;
            case RFS_SET_TEST_INT:
                cdi_not_implemented("RFS_SET_TEST_INT");
                return;
            case RFS_SET_TEST_SHORT:
                cdi_not_implemented("RFS_SET_TEST_SHORT");
                return;
            case RFS_SET_TEST_LONG:
                cdi_not_implemented("RFS_SET_TEST_LONG");
                return;
            case RFS_SET_TEST_GAIN:
                cdi_not_implemented("RFS_SET_TEST_GAIN");
                return;
            case RFS_SET_SCI_1:
                cdi_not_implemented("RFS_SET_SCI_1");
                return;
            case RFS_SET_SCI_2:
                cdi_not_implemented("RFS_SET_SCI_2");
                return;
            case RFS_SET_SCI_3:
                cdi_not_implemented("RFS_SET_SCI_3");
                return;
            case RFS_SET_SCI_4:
                cdi_not_implemented("RFS_SET_SCI_4");
                return;
            case RFS_SET_GAIN_ANA_SET:
                cdi_not_implemented("RFS_SET_GAIN_ANA_SET");
                return;
            case RFS_SET_GAIN_ANA_CFG:
                cdi_not_implemented("RFS_SET_GAIN_ANA_CFG");
                return;
            case RFS_SET_GAIN_DIG_SET:
                cdi_not_implemented("RFS_SET_GAIN_DIG_SET");
                return;
            case RFS_SET_GAIN_DIG_CFG:
                cdi_not_implemented("RFS_SET_GAIN_DIG_CFG");
                return;
            case RFS_SET_ROUTE_SET12:
                cdi_not_implemented("RFS_SET_ROUTE_SET12");
                return;
            case RFS_SET_ROUTE_SET34:
                cdi_not_implemented("RFS_SET_ROUTE_SET34");
                return;
            case RFS_SET_AVG_SET:
                state.seq.Navg1_shift = arg_low & 0x0F;
                state.seq.Navg2_shift = (arg_low & 0xF0) >> 4;
                fill_derived();
                return;
            case RFS_SET_AVG_OUTLIER:
                cdi_not_implemented("RFS_SET_AVG_OUTLIER");
                return;
            case RFS_SET_AVG_FREQ:
                state.seq.Navgf = arg_low;
                fill_derived();
                return;
            case RFS_SET_AVG_SET_HI:
                cdi_not_implemented("RFS_SET_AVG_SET_HI");
                return;
            case RFS_SET_AVGI_SET_MID:
                cdi_not_implemented("RFS_SET_AVGI_SET_MID");
                return;
            case RFS_SET_CAL_FRAC_SET:
                cdi_not_implemented("RFS_SET_CAL_FRAC_SET");
                return;
            case RFS_SET_CAL_MAX_SET:
                cdi_not_implemented("RFS_SET_CAL_MAX_SET");
                return;
            case RFS_SET_CAL_LOCK_SET:
                cdi_not_implemented("RFS_SET_CAL_LOCK_SET");
                return;
            case RFS_SET_CAL_SNR_SET:
                cdi_not_implemented("RFS_SET_CAL_SNR_SET");
                return;
            case RFS_SET_CAL_BIN_ST:
                cdi_not_implemented("RFS_SET_CAL_BIN_ST");
                return;
            case RFS_SET_CAL_BIN_EN:
                cdi_not_implemented("RFS_SET_CAL_BIN_EN");
                return;
            case RFS_SET_CAL_ANT_MASK:
                cdi_not_implemented("RFS_SET_CAL_ANT_MASK");
                return;
            case RFS_SET_ZOOM_EN:
                cdi_not_implemented("RFS_SET_ZOOM_EN");
                return;
            case RFS_SET_ZOOM_SET1:
                cdi_not_implemented("RFS_SET_ZOOM_SET1");
                return;
            case RFS_SET_ZOOM_SET1_LO:
                cdi_not_implemented("RFS_SET_ZOOM_SET1_LO");
                return;
            case RFS_SET_ZOOM_SET1_HI:
                cdi_not_implemented("RFS_SET_ZOOM_SET1_HI");
                return;
            case RFS_SET_ZOOM_SET2:
                cdi_not_implemented("RFS_SET_ZOOM_SET2");
                return;
            case RFS_SET_ZOOM_SET2_LO:
                cdi_not_implemented("RFS_SET_ZOOM_SET2_LO");
                return;
            case RFS_SET_ZOOM_SET2_HI:
                cdi_not_implemented("RFS_SET_ZOOM_SET2_HI");
                return;
            case RFS_SET_SEQ_EN:
                if (state.base.spectrometer_enable) {
                    state.base.errors |= CDI_COMMAND_WRONG;
                } else {
                    state.sequencer_enabled = (arg_low>0);
                }
                return;
            case RFS_SET_SEQ_REP:
                if (state.base.spectrometer_enable) {
                    state.base.errors |= CDI_COMMAND_WRONG;
                } else {
                    state.base.sequencer_repeat = arg_low;
                }
                return;
            case RFS_SET_SEQ_CYC:
                if (state.base.spectrometer_enable) {
                    state.base.errors |= CDI_COMMAND_WRONG;
                } else {
                    state.Nseq = arg_low;
                    state.base.sequencer_step = 0;
                }
                return;
            case RFS_SET_SEQ_STO:
                if (state.base.spectrometer_enable) {
                    state.base.errors |= CDI_COMMAND_WRONG;
                } else {
                    state.seq_program[state.base.sequencer_step] = state.seq;
                    state.seq_times[state.base.sequencer_step] = arg_low;
                    state.base.sequencer_step++;
                }
                return;
                
            default:
                cdi_not_implemented("UNRECOGNIZED COMMAND");
                return;
        } 
    }
    debug_print ("   Commmand not implemented, ignoring.\n");
    return;
}


void transfer_from_df ()
{
// Want to now transfer all 16 pks worth of data to DDR memory
    int32_t *df_ptr = (int32_t *)DF_BASE_ADDR;
    int32_t *ddr_ptr = (int32_t *)DDR3_BASE_ADDR;

    for (uint16_t i = 0; i < (NCHANNELS*NSPECTRA); i++)
    {
        *ddr_ptr += ((*df_ptr) / (1 << state.Navg2_total_shift));
        if (state.seq.Navgf == 2) {
            df_ptr++;
            *ddr_ptr += ((*df_ptr) / (1 << state.Navg2_total_shift));
        } else if (state.seq.Navgf > 2) {
            df_ptr++;
            *ddr_ptr += ((*df_ptr) / (1 << state.Navg2_total_shift));
            df_ptr++;
            *ddr_ptr += ((*df_ptr) / (1 << state.Navg2_total_shift));
            df_ptr++; // Skip the 100kHz, 200kHz, etc which are picket-fence contaminated
            if (state.seq.Navgf == 4) {
            *ddr_ptr += ((*df_ptr) / (1 << state.Navg2_total_shift));
            }
        }   
        df_ptr++;
        ddr_ptr++;
    }
    //debug_print ("Processing spectrum %i\n", avg_counter);
    if (avg_counter%100 == 0) debug_print ("Processed %i spectra\n", avg_counter); 
    avg_counter++;

    spec_clear_df_flag(); // Clear the flag to indicate that we have read the data
}


void transfer_to_cdi () {
    int32_t *ddr_ptr = (int32_t *)DDR3_BASE_ADDR;
    int32_t *cdi_ptr = (int32_t *)CDI_BASE_ADDR;
    debug_print ("Dumping averaged spectra to CDI\n");
    for (uint8_t ch = 0; ch < NSPECTRA; ch++)
    {
        while (!cdi_ready()) {}
        memcpy(cdi_ptr, ddr_ptr, state.Nfreq * sizeof(uint32_t));
        memset(ddr_ptr, 0, state.Nfreq * sizeof(uint32_t));
        debug_print("   Writing spectrum for ch %i\n",ch);
        cdi_dispatch(0x210+ch, state.Nfreq*sizeof(int32_t));
        ddr_ptr += state.Nfreq;
    }

}

void core_loop()
{
    uint8_t cmd, arg_high, arg_low;

    core_init_state();

    for (;;)
    {
        // Check if we have a new command from the CDI
        if (cdi_new_command(&cmd, &arg_high, &arg_low)) {
            if ((cmd==RFS_Settings) && (arg_high==RFS_SET_TIME_TO_DIE)) break;
            cdi_process_command(cmd, arg_high, arg_low);
        }    
        // Check if we have a new spectrum packet from the FPGA
        if (spec_new_spectrum_ready())
        {
            transfer_from_df();
            // Check if we have reached filled up Stage 2 averaging
            // and if so, push things out to CDI
            if (avg_counter == state.Navg2)
            {
                avg_counter = 0;
                // Now one by one, we will loop through the packets placed in DDR Memory
                // For each channel, set the APID, send it to the SRAM
                // Then check to see if this software or the client will control the CDI writes
                transfer_to_cdi();
                if (state.sequencer_enabled) {
                    state.base.sequencer_substep--;
                    if (state.base.sequencer_substep == 0) {
                        state.base.sequencer_step = (state.base.sequencer_step+1)%state.Nseq;
                        if (state.base.sequencer_step == 0) {
                            state.base.sequencer_counter++;
                            debug_print("Starting sequencer cycle # %i/%i\n", state.base.sequencer_counter+1, state.base.sequencer_repeat);
                            if ((state.base.sequencer_repeat>0) & (state.base.sequencer_counter == state.base.sequencer_repeat)) {
                                debug_print("Sequencer done.\n");
                                RFS_stop();
                            }
                        }
                        state.base.sequencer_substep = state.seq_times[state.base.sequencer_step];
                        state.seq = state.seq_program[state.base.sequencer_step];
                        fill_derived();
                        set_spectrometer_to_sequencer();
                    }
                }

            }


            // make sure we have done this in time
            if (spec_df_flag())
            {
                debug_print("Error: missed a spectrum packet\n");
            }
        }
    }
}