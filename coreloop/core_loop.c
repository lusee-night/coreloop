#include <string.h> // Add include for memcpy
#include <stdlib.h>
#include <stdint.h>
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
uint8_t housekeeping_request;
uint8_t range_adc, resettle; 
bool tick_tock;
uint32_t heartbeat_counter;
uint32_t resettle_counter;

/**************************************************/



/************* CODE STARTS HERE **************************************/
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))




void cdi_not_implemented(const char *msg)
{
    debug_print("CDI command not implemented: ");
    debug_print(msg);
    debug_print("\n\r")
    exit(1);
    return;
}


static inline void new_unique_packet_id()
{
    unique_packet_id++;
}



static inline void update_time() {
     //spec_get_time(&state.base.time_seconds, &state.base.time_subseconds);
}

void send_hello_packet() {
    debug_print ("Sending hello packet.\n\r")
    struct startup_hello *payload = (struct startup_hello*) (TLM_BUF);
    new_unique_packet_id();
    update_time();
    wait_for_cdi_ready();
    payload->version = VERSION_ID;
    payload->unique_packet_id = unique_packet_id;
    payload->time_seconds = state.base.time_seconds;
    payload->time_subseconds = state.base.time_subseconds;
    cdi_dispatch(AppID_uC_Start, sizeof(struct startup_hello));
}

void process_hearbeat() {
    if (heartbeat_counter > 0) return;
    debug_print("Sending heartbeat.\n\r");
    char *msg = (char *) TLM_BUF;
    msg[0] = 'B';
    msg[1] = 'R';
    msg[2] = 'N';
    msg[3] = 'M';
    msg[4] = 'R';
    msg[5] = 'L';    
    cdi_dispatch(AppID_uC_HeartBeat, 6);
    heartbeat_counter = HEARTBEAT_DELAY;
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
    housekeeping_request = 0;
    range_adc = 0;
    for (int i=0; i<NINPUT; i++) state.base.actual_gain[i] = GAIN_MED;
    for (int i=0; i<NSPECTRA; i++) state.base.actual_bitslice[i] = MIN(state.seq.bitslice[i],0x1F); // to convert FF to 16
    spec_set_spectrometer_enable(false);
    state.base.sequencer_step = 0xFF;
    state.sequencer_enabled = false;
    state.Nseq = 0;
    state.cdi_dispatch.prod_count = 0xFF; // >0F so disabled.
    tick_tock = true;
    update_time();
    unique_packet_id = state.base.time_seconds;
    fill_derived();
    set_spectrometer_to_sequencer();
    heartbeat_counter = HEARTBEAT_DELAY;
}

void reset_errormasks() {
    state.base.errors = 0;
    state.base.spec_overflow = 0;
    state.base.notch_overflow = 0;
}


void RFS_stop() {
    debug_print ("Stopping spectrometer\n\r");
    state.base.spectrometer_enable = false;
    spec_set_spectrometer_enable(false);
}

void RFS_start() {
    debug_print ("Starting spectrometer\n\r");
    state.base.spectrometer_enable = true;
    avg_counter = 0;
    memset((void *)SPEC_TICK, 0, NCHANNELS * sizeof(uint32_t));
    memset((void *)SPEC_TOCK, 0, NCHANNELS * sizeof(uint32_t));
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

void trigger_ADC_stat() {
    spec_trigger_ADC_stat(ADC_STAT_SAMPLES);
}

void update_spec_gains() {
    for (int i = 0; i < NINPUT; i++) {
        spec_set_gain(i, state.base.actual_gain[i]);
    }
    debug_print("gains changed\n");
}

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


inline static bool process_cdi()
{
    uint8_t cmd, arg_high, arg_low;
    uint8_t ch, xcor, val;
    uint8_t ant1low, ant1high, ant2low, ant2high, ant3low, ant3high, ant4low, ant4high;
    if (!cdi_new_command(&cmd, &arg_high, &arg_low)) return false;
    debug_print ("Got new CDI command.\n\r")
    if (cmd==RFS_Settings)  {
        switch (arg_high) {
            case RFS_SET_START:
                RFS_start();
                break;
            case RFS_SET_STOP:
                RFS_stop();
                break;                
            case RFS_SET_RESET:
                RFS_stop();
                spec_set_reset();
                core_init_state();
                break;
            case RFS_SET_STORE:
                spec_store();
                break;
            case RFS_SET_RECALL:
                spec_recall();
                break;
            case RFS_SET_HK_REQ:            
                if (arg_low < 2) {
                    housekeeping_request = 1+arg_low; 
                } else {
                    state.base.errors |= CDI_COMMAND_BAD_ARGS;
                }
                break;
            case RFS_SET_ADC:
                cdi_not_implemented("RFS_SET_ADC");
                break;
            case RFS_SET_RANGE_ADC:
                range_adc = 1;
                trigger_ADC_stat();           
                break;
            
            case RFS_SET_TIME_TO_DIE:
                debug_print("Recevied time-to-die.\n\r")
                return true;

           case RFS_SET_TEST:
                cdi_not_implemented("RFS_SET_TEST");
                break;
            case RFS_SET_SCIENCE:
                cdi_not_implemented("RFS_SET_SCIENCE");
                break;
            case RFS_SET_LOAD_FL:
                cdi_not_implemented("RFS_SET_LOAD_FL");
                break;
            case RFS_SET_STORE_FL:
                cdi_not_implemented("RFS_SET_STORE_FL");
                break;

            case RFS_SET_GAIN_ANA_SET:                
                for (int i=0; i<NINPUT; i++){
                    uint8_t val = (arg_low >> (2*i)) & 0x03;
                    state.seq.gain[i] = val;
                    if (val!=GAIN_AUTO) state.base.actual_gain[i] = val;
                }
                update_spec_gains();
                break;
            case RFS_SET_GAIN_ANA_CFG_MIN:
                ch = arg_low & 0x03;
                val = (arg_low & 0xFC) >> 2;
                state.seq.gain_auto_min[ch] = 16*val; //max 16*64 = 1024, which is 1/8th
                break;
            case RFS_SET_GAIN_ANA_CFG_MULT:
                ch = arg_low & 0x03;
                val = (arg_low & 0xFC) >> 2;
                state.seq.gain_auto_mult[ch] = val;
                break;
            case RFS_SET_BITSLICE_LOW:
                xcor = arg_low & 0x07;
                val = (arg_low & 0xF8) >> 3;
                state.seq.bitslice[xcor] = val;
                break;
            case RFS_SET_BITSLICE_HIGH:
                xcor = (arg_low & 0x07) + 8;
                val = (arg_low & 0xF8) >> 3;
                state.seq.bitslice[xcor] = val;
                break;
            case RFS_SET_BITSLICE_AUTO:
                if (arg_low > 0) {
                    for (int i=0; i<NSPECTRA; i++) state.seq.bitslice[i] = 0xFF;
                    state.seq.bitslice_keep_bits = arg_low;
                } else {
                    for (int i=0; i<NSPECTRA; i++) state.seq.bitslice[i] = 0x1F;
                }
                break;

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
                break;
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
                break;


            case RFS_SET_AVG_SET:
                state.seq.Navg1_shift = arg_low & 0x0F;
                state.seq.Navg2_shift = (arg_low & 0xF0) >> 4;
                fill_derived();
                break;
            case RFS_SET_AVG_OUTLIER:
                cdi_not_implemented("RFS_SET_AVG_OUTLIER");
                break;
            case RFS_SET_AVG_FREQ:
                state.seq.Navgf = arg_low;
                fill_derived();
                break;
            case RFS_SET_AVG_NOTCH:
                state.seq.notch = arg_low;
                break;
            case RFS_SET_AVG_SET_HI:
                cdi_not_implemented("RFS_SET_AVG_SET_HI");
                break;
            case RFS_SET_AVGI_SET_MID:
                cdi_not_implemented("RFS_SET_AVGI_SET_MID");
                break;
            case RFS_SET_OUTPUT_FORMAT:
                if (arg_low > 2) {
                    state.base.errors |= CDI_COMMAND_BAD_ARGS;
                } else {
                    state.seq.format = arg_low;
                }
                break;
            case RFS_SET_CAL_FRAC_SET:
                cdi_not_implemented("RFS_SET_CAL_FRAC_SET");
                break;
            case RFS_SET_CAL_MAX_SET:
                cdi_not_implemented("RFS_SET_CAL_MAX_SET");
                break;
            case RFS_SET_CAL_LOCK_SET:
                cdi_not_implemented("RFS_SET_CAL_LOCK_SET");
                break;
            case RFS_SET_CAL_SNR_SET:
                cdi_not_implemented("RFS_SET_CAL_SNR_SET");
                break;
            case RFS_SET_CAL_BIN_ST:
                cdi_not_implemented("RFS_SET_CAL_BIN_ST");
                break;
            case RFS_SET_CAL_BIN_EN:
                cdi_not_implemented("RFS_SET_CAL_BIN_EN");
                break;
            case RFS_SET_CAL_ANT_MASK:
                cdi_not_implemented("RFS_SET_CAL_ANT_MASK");
                break;
            case RFS_SET_ZOOM_EN:
                cdi_not_implemented("RFS_SET_ZOOM_EN");
                break;
            case RFS_SET_ZOOM_SET1:
                cdi_not_implemented("RFS_SET_ZOOM_SET1");
                break;
            case RFS_SET_ZOOM_SET1_LO:
                cdi_not_implemented("RFS_SET_ZOOM_SET1_LO");
                break;
            case RFS_SET_ZOOM_SET1_HI:
                cdi_not_implemented("RFS_SET_ZOOM_SET1_HI");
                break;
            case RFS_SET_ZOOM_SET2:
                cdi_not_implemented("RFS_SET_ZOOM_SET2");
                break;
            case RFS_SET_ZOOM_SET2_LO:
                cdi_not_implemented("RFS_SET_ZOOM_SET2_LO");
                break;
            case RFS_SET_ZOOM_SET2_HI:
                cdi_not_implemented("RFS_SET_ZOOM_SET2_HI");
                break;
            case RFS_SET_SEQ_EN:
                if (state.base.spectrometer_enable) {
                    state.base.errors |= CDI_COMMAND_BAD;
                } else {
                    state.sequencer_enabled = (arg_low>0);
                }
                break;
            case RFS_SET_SEQ_REP:
                if (state.base.spectrometer_enable) {
                    state.base.errors |= CDI_COMMAND_BAD;
                } else {
                    state.base.sequencer_repeat = arg_low;
                }
                break;
            case RFS_SET_SEQ_CYC:
                if (state.base.spectrometer_enable) {
                    state.base.errors |= CDI_COMMAND_BAD;
                } else {
                    state.Nseq = arg_low;
                    state.base.sequencer_step = 0;
                }
                break;
            case RFS_SET_SEQ_STO:
                if (state.base.spectrometer_enable) {
                    state.base.errors |= CDI_COMMAND_BAD;
                } else {
                    state.seq_program[state.base.sequencer_step] = state.seq;
                    state.seq_times[state.base.sequencer_step] = arg_low;
                    state.base.sequencer_step++;
                }
                break;
                
            default:
                debug_print ("UNRECOGNIZED RFS_SET COMMAND\n\r");
                state.base.errors |= CDI_COMMAND_UNKNOWN;
                break;
        } 
    } else {
        debug_print ("   Commmand not implemented, ignoring.\n\r");
        state.base.errors |= CDI_COMMAND_UNKNOWN;
    }
    return false;
}


bool analog_gain_control() {

    bool gains_changed = false;
    
    for (int i = 0; i < NINPUT; i++) {
        if (state.seq.gain[i] != GAIN_AUTO) continue; // Don't do anything unless AGC is enabled
        int32_t cmax = MAX(state.base.ADC_stat[i].max, -state.base.ADC_stat[i].min);
        //debug_print("AGC: Channel %i max = %i (%i %i) \n", i, cmax, state.gain_auto_max[i], state.seq.gain_auto_min[i]);
        if (cmax > state.gain_auto_max[i]) {
            if (state.base.actual_gain[i] > GAIN_LOW) {
                state.base.actual_gain[i] --;
                state.base.errors |= ((ANALOG_AGC_ACTION_CH1) << i);
                gains_changed = true;
            } else {
                state.base.errors |= ANALOG_AGC_TOO_HIGH;
            }
        } else if (cmax < state.seq.gain_auto_min[i]) {
            if (state.base.actual_gain[i] < GAIN_HIGH) {
                state.base.actual_gain[i] ++;
                state.base.errors |= ((ANALOG_AGC_ACTION_CH1) << i);
                gains_changed = true;
            } else {
                state.base.errors |= ANALOG_AGC_TOO_LOW;
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
            debug_print("Gains changed, resettle\n\r");
            resettle = true;
            resettle_counter = RESETTLE_DELAY;
        } else {
            if (range_adc) {
                range_adc = 0;
                housekeeping_request = 2;
            }
        }
    }
    if ((resettle) & (resettle_counter == 0)) {
        trigger_ADC_stat();
        resettle = false;
        if (state.base.spectrometer_enable) {
            restart_spectrometer();
        }
    }
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
    int32_t *df_ptr = (int32_t *)SPEC_BUF;
    int32_t *ddr_ptr = tick_tock ? (int32_t *)(SPEC_TICK) : (int32_t *)(SPEC_TOCK);
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
    //debug_print ("Processing spectrum %i %i %i %i\n", avg_counter, sp, leading_zeros_min[sp], leading_zeros_max[sp]);
    }
    
    //if (avg_counter%100 == 0) debug_print ("Processed 100 spectra...\n");
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

void process_housekeeping() {
    if (housekeeping_request == 0) return;
    housekeeping_request--;
    switch (housekeeping_request) {
        case 0:
            debug_print ("Sending housekeeping type 0\n\r");
            struct housekeeping_data_0 *hk0 = (struct housekeeping_data_0 *)TLM_BUF;
            wait_for_cdi_ready();
            hk0->version = VERSION_ID;
            hk0->unique_packet_id = unique_packet_id;
            hk0->housekeeping_type = 0;
            hk0->core_state = state;
            cdi_dispatch(AppID_uC_Housekeeping, sizeof(struct housekeeping_data_0));
            break;

        case 1:
            debug_print ("Sending housekeeping type 1\n\r");
            struct housekeeping_data_1 *hk1 = (struct housekeeping_data_1 *)TLM_BUF;
            wait_for_cdi_ready();
            hk1->version = VERSION_ID;
            hk1->unique_packet_id = unique_packet_id;
            hk1->housekeeping_type = 1;
            for (int i=0; i<NINPUT; i++) {
                hk1->ADC_stat[i] = state.base.ADC_stat[i];
                hk1->actual_gain[i] = state.base.actual_gain[i];
            }
            cdi_dispatch(AppID_uC_Housekeeping, sizeof(struct housekeeping_data_1));
            break;
    }
    housekeeping_request = 0;
}


void send_metadata_packet() {
    struct meta_data *meta = (struct meta_data *)TLM_BUF;
    wait_for_cdi_ready();
    meta->version = VERSION_ID;
    meta->unique_packet_id = unique_packet_id;
    meta->seq = state.seq;
    meta->base = state.base;
    cdi_dispatch(AppID_MetaData, sizeof(struct meta_data));
    reset_errormasks();
}


void dispatch_32bit_data() {
    // if we are in tick, we are copyng over TOCK, otherwise TICK !!
    int32_t *ddr_ptr = tick_tock ? (int32_t *)(SPEC_TOCK) : (int32_t *)(SPEC_TICK);
    ddr_ptr += state.cdi_dispatch.prod_count * state.Nfreq;
    int32_t *cdi_ptr = (int32_t *)TLM_BUF;
    int32_t *crc_ptr;

    *cdi_ptr = (int32_t)(state.cdi_dispatch.packet_id);
    cdi_ptr++;
    crc_ptr = cdi_ptr;
    cdi_ptr++;
    size_t data_size = state.Nfreq*sizeof(int32_t);
    size_t packet_size = data_size+2*sizeof(int32_t);
    wait_for_cdi_ready();
    memcpy(cdi_ptr, ddr_ptr, state.Nfreq * sizeof(uint32_t));
    memset(ddr_ptr, 0, state.Nfreq * sizeof(uint32_t));
    *crc_ptr = CRC(cdi_ptr, data_size);
    cdi_dispatch(state.cdi_dispatch.appId, packet_size);
}

void dispatch_16bit_updates_data() {
    cdi_not_implemented("16bit w updates data format");    
}

void dispatch_16bit_float1_data() {
    cdi_not_implemented("16bit w float1 data format");    
}

void transfer_to_cdi () {
    debug_print ("Sending averaged spectra to CDI.\n\r");
    new_unique_packet_id();
    update_time();
    spec_get_TVS(state.base.TVS_sensors);
    send_metadata_packet();
    state.cdi_dispatch.int_counter = DISPATCH_DELAY; // 10*0.01s ~10 Hz
    state.cdi_dispatch.prod_count = 0; // 
    state.cdi_dispatch.appId = AppID_SpectraHigh; // fix
    state.cdi_dispatch.format = state.seq.format;
    state.cdi_dispatch.packet_id = unique_packet_id;
}

void process_delayed_cdi_dispatch() {
    if (state.cdi_dispatch.int_counter > 0) return;
    if (state.cdi_dispatch.prod_count > 0x0F)  return;
    switch (state.cdi_dispatch.format) {
        case OUTPUT_32BIT:
            dispatch_32bit_data();
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
    state.cdi_dispatch.prod_count++;
    state.cdi_dispatch.appId++;
    state.cdi_dispatch.int_counter = DISPATCH_DELAY;
}

void advance_sequencer() {

state.base.sequencer_substep--;
if (state.base.sequencer_substep == 0) {
    state.base.sequencer_step = (state.base.sequencer_step+1)%state.Nseq;
    if (state.base.sequencer_step == 0) {
        state.base.sequencer_counter++;
        if ((state.base.sequencer_repeat>0) & (state.base.sequencer_counter == state.base.sequencer_repeat)) {
            //debug_print("Sequencer done.\n");
            RFS_stop();
        } else {
            //debug_print("Starting sequencer cycle # %i/%i\n", state.base.sequencer_counter+1, state.base.sequencer_repeat);
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

static inline void process_spectrometer() {
// Check if we have a new spectrum packet from the FPGA
if (spec_new_spectrum_ready())
    {
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
        if (avg_counter == state.Navg2)
        {
            avg_counter = 0;
            tick_tock = !tick_tock;
            // Now one by one, we will loop through the packets placed in DDR Memory
            // For each channel, set the APID, send it to the SRAM
            // Then check to see if this software or the client will control the CDI writes
            transfer_to_cdi();
            if (state.sequencer_enabled) advance_sequencer();

        }
    }
}

uint32_t dwait = 0;

void core_loop()
{
    send_hello_packet();
    core_init_state();

    for (;;)
    {
        update_time();
        // Check if we have a new CDI command and process it.
        // If this functions returns true, it means we got the time-to-die command
        if (process_cdi()) break;
        process_spectrometer();
        process_delayed_cdi_dispatch();
        process_gain_range();
        process_housekeeping();
        process_hearbeat();
        //debug_print("still alive.\n\r")

#ifdef NOTREAL
        // if we are running inside the coreloop test harness.
      MSYS_EI4_IRQHandler();

#else
        dwait ++;
        if (dwait == 256) {
            if (resettle_counter > 0) resettle_counter--;
            if (state.cdi_dispatch.int_counter > 0) state.cdi_dispatch.int_counter--;
            if (heartbeat_counter > 0) heartbeat_counter--;
            dwait=0;
        }

#endif
    }
}

uint8_t MSYS_EI4_IRQHandler(void)
{

    /* Clear the interrupt within the timer */
    if (resettle_counter > 0) resettle_counter--;   
    if (state.cdi_dispatch.int_counter > 0) state.cdi_dispatch.int_counter--;
    if (heartbeat_counter > 0) heartbeat_counter--;
    TMR_clear_int(&g_core_timer_0);
    return (EXT_IRQ_KEEP_ENABLED);
}
