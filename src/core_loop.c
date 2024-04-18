#include <string.h> // Add include for memcpy
#include <stdlib.h>
#include <stdint.h>
#include "printf.h"
#include "LuSEE_IO.h"
#include "core_loop.h"
#include "spectrometer_interface.h"
#include "cdi_interface.h"
// autogenerated include files from documentation
#include "lusee_commands.h"
#include "lusee_appIds.h"


/***************** GLOBAL STATE ******************/

struct core_state state;
uint16_t avg_counter = 0;
uint32_t unique_packet_id;
uint8_t leading_zeros_min[NSPECTRA];
uint8_t leading_zeros_max[NSPECTRA];

/**************************************************/



/************* CODE STARTS HERE **************************************/
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))




void cdi_not_implemented(const char *msg)
{
    debug_print("CDI command not implemented: %s\n",msg);
    exit(1);
    return;
}


static inline void new_unique_packet_id()
{
    unique_packet_id++;
}



static inline void update_time() {
     spec_get_time(&state.base.time_seconds, &state.base.time_subseconds);
}

void send_hello_packet() {
    struct startup_hello *payload = (struct startup_hello*) (CDI_BASE_ADDR);
    new_unique_packet_id();
    update_time();
    wait_for_cdi_ready();
    payload->version = VERSION_ID;
    payload->unique_packet_id = unique_packet_id;
    payload->time_seconds = state.base.time_seconds;
    payload->time_subseconds = state.base.time_subseconds;
    cdi_dispatch(AppID_uC_Start, sizeof(struct startup_hello));

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
    seq->Navg1_shift = 11;
    seq->Navg2_shift = 9;
    seq->Navgf = 1;
    for (int i = 0; i < NSPECTRA; i++) seq->bitslice[i]=0x1F;
    seq->notch = 0;
    seq->bitslice_keep_bits=13;
    seq->format = OUTPUT_16BIT_UPDATES;

}

void fill_derived() {
    state.Navg1 = 1 << state.seq.Navg1_shift;
    state.Navg2 = 1 << state.seq.Navg2_shift;
    // total shift takes into account frequency averaging;
    state.Navg2_total_shift = state.seq.Navg2_shift;
    state.Nfreq = NCHANNELS; 
    if (state.seq.Navgf == 2 ) { state.Navg2_total_shift += 1; state.Nfreq = NCHANNELS/2; }
    if ((state.seq.Navgf == 3 ) || (state.seq.Navgf == 4)) { state.Navg2_total_shift += 2; state.Nfreq = NCHANNELS/4;} 
    for (int i=0; i<NINPUT; i++) {
        state.gain_auto_max[i] = (state.seq.gain_auto_min[i] * state.seq.gain_auto_mult[i]);
    }   
}

void core_init_state(){   
    default_seq (&state.seq);
    state.base.errors = 0;
    state.base.spectrometer_enable = false;
    for (int i=0; i<NINPUT; i++) state.base.actual_gain[i] = GAIN_MED;
    for (int i=0; i<NSPECTRA; i++) state.base.actual_bitslice[i] = MIN(state.seq.bitslice[i],0x1F); // to convert FF to 16
    spec_set_spectrometer_enable(false);
    state.base.sequencer_step = 0xFF;
    state.sequencer_enabled = false;
    state.Nseq = 0;
    update_time();
    unique_packet_id = state.base.time_seconds;
    fill_derived();
    set_spectrometer_to_sequencer();
}

void reset_errormasks() {
    state.base.errors = 0;
    state.base.spec_overflow = 0;
    state.base.notch_overflow = 0;
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


void restart_spectrometer()
{
    RFS_stop();
    RFS_start();
}

bool restart_needed (struct sequencer_state *seq1, struct sequencer_state *seq2 ) {
    
    if (seq1->notch != seq2->notch) return true;
    for (int i=0; i<NINPUT; i++) { 
        if (seq1->gain[i] != seq2->gain[i]) return true;
        if (seq1->route[i].plus != seq2->route[i].plus) return true;
        if (seq1->route[i].minus != seq2->route[i].minus) return true;
    }
    for (int i=0; i<NSPECTRA; i++) if (seq1->bitslice[i] != seq2->bitslice[i]) return true;
}


void cdi_process_command(uint8_t cmd, uint8_t arg_high, uint8_t arg_low)
{
    // Do something with the command
    uint8_t ch, xcor, val;
    uint8_t ant1low, ant1high, ant2low, ant2high, ant3low, ant3high, ant4low, ant4high;
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
                for (int i=0; i<NINPUT; i++){
                    uint8_t val = (arg_low >> (2*i)) & 0x03;
                    state.seq.gain[i] = val;
                    if (val!=GAIN_AUTO) state.base.actual_gain[i] = val;
                }
                return;
            case RFS_SET_GAIN_ANA_CFG_MIN:
                ch = arg_low & 0x03;
                val = (arg_low & 0xFC) >> 2;
                state.seq.gain_auto_min[ch] = 16*val; //max 16*64 = 1024, which is 1/8th
                return;
            case RFS_SET_GAIN_ANA_CFG_MULT:
                ch = arg_low & 0x03;
                val = (arg_low & 0xFC) >> 2;
                state.seq.gain_auto_mult[ch] = val;
                return;
            case RFS_SET_BITSLICE_LOW:
                xcor = arg_low & 0x07;
                val = (arg_low & 0xF8) >> 3;
                state.seq.bitslice[xcor] = val;
                return;
            case RFS_SET_BITSLICE_HIGH:
                xcor = (arg_low & 0x07) + 8;
                val = (arg_low & 0xF8) >> 3;
                state.seq.bitslice[xcor] = val;
                return;
            case RFS_SET_BITSLICE_AUTO:
                if (arg_low > 0) {
                    for (int i=0; i<NSPECTRA; i++) state.seq.bitslice[i] = 0xFF;
                    state.seq.bitslice_keep_bits = arg_low;
                } else {
                    for (int i=0; i<NSPECTRA; i++) state.seq.bitslice[i] = 0x1F;
                }
                return;

            case RFS_SET_ROUTE_SET12:
                ant2low = arg_low & 0x03;
                ant2high = (arg_low & 0x0C) >> 2;
                ant1low = (arg_low & 0x30) >> 4;
                ant1high = (arg_low & 0xC0) >> 6;
                if (ant2low == ant2high) ant2low = 0xFF;
                if (ant1low == ant1high) ant1low = 0xFF;
                state.seq.route[0].plus = ant1high;
                state.seq.route[0].minus = ant1low;
                state.seq.route[1].plus = ant2high;
                state.seq.route[1].minus = ant2low;
                return;
            case RFS_SET_ROUTE_SET34:
                ant4low = arg_low & 0x03;
                ant4high = (arg_low & 0x0C) >> 2;
                ant3low = (arg_low & 0x30) >> 4;
                ant3high = (arg_low & 0xC0) >> 6;
                if (ant4low == ant4high) ant4low = 0xFF;
                if (ant3low == ant3high) ant3low = 0xFF;
                state.seq.route[2].plus = ant3high;
                state.seq.route[2].minus = ant3low;
                state.seq.route[3].plus = ant4high;
                state.seq.route[3].minus = ant4low;
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
            case RFS_SET_AVG_NOTCH:
                state.seq.notch = arg_low;
                return;

            case RFS_SET_AVG_SET_HI:
                cdi_not_implemented("RFS_SET_AVG_SET_HI");
                return;
            case RFS_SET_AVGI_SET_MID:
                cdi_not_implemented("RFS_SET_AVGI_SET_MID");
                return;
            case RFS_SET_OUTPUT_FORMAT:
                if (arg_low > 2) {
                    state.base.errors |= CDI_COMMAND_WRONG_ARGS;
                } else {
                    state.seq.format = arg_low;
                }
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


bool analog_gain_control() {

    bool gains_changed = false;
    
    spec_get_ADC_stat(state.base.ADC_stat);
    for (int i = 0; i < NINPUT; i++) {
        if (state.seq.gain[i] != GAIN_AUTO) continue; // Don't do anything unless AGC is enabled
        int32_t cmax = MAX(state.base.ADC_stat[i].max, -state.base.ADC_stat[i].min);
        //debug_print("AGC: Channel %i max = %i (%i %i) \n", i, cmax, state.gain_auto_max[i], state.seq.gain_auto_min[i]);
        if (cmax > state.gain_auto_max[i]) {
            if (state.base.actual_gain[i] > GAIN_LOW) {
                state.base.actual_gain[i] --;
                state.base.errors |= ((ANALOG_AGC_ACTION_CH1) << i);
                gains_changed = true;
                debug_print("AGC: Channel %i gain decreased to %i\n", i, state.base.actual_gain[i]);
            } else {
                state.base.errors |= ANALOG_AGC_TOO_HIGH;
            }
        } else if (cmax < state.seq.gain_auto_min[i]) {
            if (state.base.actual_gain[i] < GAIN_HIGH) {
                state.base.actual_gain[i] ++;
                state.base.errors |= ((ANALOG_AGC_ACTION_CH1) << i);
                gains_changed = true;
                debug_print("AGC: Channel %i gain increased to %i\n", i, state.base.actual_gain[i]);
            } else {
                state.base.errors |= ANALOG_AGC_TOO_LOW;
            }
        }
    }
    return gains_changed;
}

 
  
bool bitslice_control() {
    bool bitslice_changed = false;
    int32_t *ddr_ptr = (int32_t *) DDR3_BASE_ADDR;
    for (int i = 0; i < NSPECTRA; i++) {
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
    return bitslice_changed;
}

int32_t get_with_zeros(int32_t val, uint8_t *min, uint8_t *max) {
    int32_t zeros = __builtin_clz(abs(val));
    *min = MIN(*min, zeros);
    *max = MAX(*max, zeros);
    return val;
}


void transfer_from_df ()
{
// Want to now transfer all 16 pks worth of data to DDR memory
    int32_t *df_ptr = (int32_t *)DF_BASE_ADDR;
    int32_t *ddr_ptr = (int32_t *)DDR3_BASE_ADDR;

    for (uint16_t sp = 0; sp< NSPECTRA; sp++) {
        leading_zeros_min[sp] = 32;
        leading_zeros_max[sp] = 0;
        for (uint16_t i = 0; i < NCHANNELS; i++) {
        *ddr_ptr += (get_with_zeros(*df_ptr,&leading_zeros_min[sp], &leading_zeros_max[sp]) / (1 << state.Navg2_total_shift));
        if (state.seq.Navgf == 2) {
            df_ptr++;
            *ddr_ptr += (get_with_zeros(*df_ptr,&leading_zeros_min[sp], &leading_zeros_max[sp]) / (1 << state.Navg2_total_shift));
        } else if (state.seq.Navgf > 2) {
            df_ptr++;
            *ddr_ptr += (get_with_zeros(*df_ptr,&leading_zeros_min[sp], &leading_zeros_max[sp]) / (1 << state.Navg2_total_shift));
            df_ptr++;
            *ddr_ptr += (get_with_zeros(*df_ptr,&leading_zeros_min[sp], &leading_zeros_max[sp]) / (1 << state.Navg2_total_shift));
            df_ptr++; // Skip the 100kHz, 200kHz, etc which are picket-fence contaminated
            if (state.seq.Navgf == 4) {
            *ddr_ptr += (get_with_zeros(*df_ptr,&leading_zeros_min[sp], &leading_zeros_max[sp]) / (1 << state.Navg2_total_shift));
            }
        }   
        df_ptr++;
        ddr_ptr++;
        }
    debug_print ("Processing spectrum %i %i %i %i\n", avg_counter, sp, leading_zeros_min[sp], leading_zeros_max[sp]);
    }
    
    if (avg_counter%100 == 0) debug_print ("Processed %i spectra\n", avg_counter); 
    avg_counter++;

}



uint32_t CRC(const void* data, size_t size) {
    const uint8_t* bytes = (const uint8_t*)data;
    uint32_t crc = 0xFFFFFFFF;

    for (size_t i = 0; i < size; i++) {
        crc ^= bytes[i];
        for (int j = 0; j < 8; j++) {
            if (crc & 1) {
                crc = (crc >> 1) ^ 0xEDB88320;
            } else {
                crc >>= 1;
            }
        }
    }

    return ~crc;
}

void send_metadata_packet() {
    struct meta_data *meta = (struct meta_data *)CDI_BASE_ADDR;
    wait_for_cdi_ready();
    meta->version = VERSION_ID;
    meta->unique_packet_id = unique_packet_id;
    meta->seq = state.seq;
    meta->base = state.base;
    cdi_dispatch(AppID_MetaData, sizeof(struct meta_data));
}


void dispatch_32bit_data(uint32_t base_appid) {
    int32_t *ddr_ptr = (int32_t *)DDR3_BASE_ADDR;
    int32_t *cdi_ptr = (int32_t *)CDI_BASE_ADDR;
    int32_t *crc_ptr;

    *cdi_ptr = (int32_t)(unique_packet_id);
    cdi_ptr++;
    crc_ptr = cdi_ptr;
    cdi_ptr++;
    size_t data_size = state.Nfreq*sizeof(int32_t);
    size_t packet_size = data_size+2*sizeof(int32_t);

    for (uint8_t ch = 0; ch < NSPECTRA; ch++)
    {
        wait_for_cdi_ready();
        memcpy(cdi_ptr, ddr_ptr, state.Nfreq * sizeof(uint32_t));
        memset(ddr_ptr, 0, state.Nfreq * sizeof(uint32_t));
        *crc_ptr = CRC(cdi_ptr, data_size);
        debug_print("   Writing spectrum for ch %i\n",ch);
        cdi_dispatch(base_appid+ch, packet_size);
        ddr_ptr += state.Nfreq;
    }
}

void dispatch_16bit_updates_data() {
    cdi_not_implemented("16bit w updates data format");    
}

void dispatch_16bit_float1_data() {
    cdi_not_implemented("16bit w float1 data format");    
}

void transfer_to_cdi () {
    debug_print ("Dumping averaged spectra to CDI\n");      
    new_unique_packet_id();
    update_time();
    send_metadata_packet();
    uint32_t base_appid = AppID_SpectraHigh; // fix

    switch (state.seq.format) {
        case OUTPUT_32BIT:
            dispatch_32bit_data(base_appid);
            break;
        case OUTPUT_16BIT_UPDATES:
            dispatch_16bit_updates_data();
            break;
        case OUTPUT_16BIT_FLOAT1:
            dispatch_16bit_float1_data();
            break;
        default:
            cdi_not_implemented("Unsupported output format");
            break;
    }
}

void core_loop()
{
    uint8_t cmd, arg_high, arg_low;

    core_init_state();

    for (;;)
    {
        update_time();
        // Check if we have a new command from the CDI
        if (cdi_new_command(&cmd, &arg_high, &arg_low)) {
            if ((cmd==RFS_Settings) && (arg_high==RFS_SET_TIME_TO_DIE)) {
                debug_print("Received time to die command. \n");   
                break;
            }
            cdi_process_command(cmd, arg_high, arg_low);
        }    
        // Check if we have a new spectrum packet from the FPGA
        if (spec_new_spectrum_ready())
        {
            // execute analog gain control and restart spectrometer if needed
            bool gains_changed = analog_gain_control();
            if (gains_changed){
                spec_clear_df_flag(); // Clear the flag to indicate that we have read the data
                restart_spectrometer();
            } else {
                transfer_from_df();
                uint16_t corr_owf, notch_owf;
                spec_get_digital_overflow(&corr_owf, &notch_owf);
                state.base.spec_overflow |= corr_owf;
                state.base.notch_overflow |= notch_owf;

                spec_clear_df_flag(); // Clear the flag to indicate that we have read the data
                // Check if we have reached filled up Stage 2 averaging
                // and if so, push things out to CDI
                bool bit_slice_changed = bitslice_control();
                if (bit_slice_changed) {
                    restart_spectrometer(); // Restart the spectrometer if the bit slice has changed; avg_counter will be reset so we don't need to worry about triggering the CDI write
                }

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
                                if ((state.base.sequencer_repeat>0) & (state.base.sequencer_counter == state.base.sequencer_repeat)) {
                                    debug_print("Sequencer done.\n");
                                    RFS_stop();
                                } else {
                                    debug_print("Starting sequencer cycle # %i/%i\n", state.base.sequencer_counter+1, state.base.sequencer_repeat);
                                }
                            }
                            
                            state.base.sequencer_substep = state.seq_times[state.base.sequencer_step];
                            bool restart = restart_needed(&state.seq, &state.seq_program[state.base.sequencer_step]); 
                            if (restart) RFS_stop();
                            state.seq = state.seq_program[state.base.sequencer_step];
                            fill_derived();
                            set_spectrometer_to_sequencer();
                            if (restart) RFS_start();
                        }
                    }
                }
            }
            // make sure we have done this in time
        }
    }
}
