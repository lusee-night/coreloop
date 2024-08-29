#include "lusee_commands.h"
#include "spectrometer_interface.h"
#include "cdi_interface.h"
#include "core_loop.h"
#include <stdlib.h>
#include <stdint.h>
#include "flash_interface.h"
#include "LuSEE_IO.h"


void cdi_not_implemented(const char *msg)
{
    debug_print("\r\nCDI command not implemented: ");
    debug_print(msg);
    return;
}



bool process_cdi()
{
    uint8_t cmd, arg_high, arg_low;
    uint8_t ch, xcor, val;
    uint8_t ant1low, ant1high, ant2low, ant2high, ant3low, ant3high, ant4low, ant4high;
    if (!cdi_new_command(&cmd, &arg_high, &arg_low)) return false;
    debug_print ("\r\nGot new CDI command: cmd = ");
    debug_print_hex(cmd);
    debug_print(", arg_hi = ");
    debug_print_hex(arg_high);
    debug_print(", arg_lo = ");
    debug_print_hex(arg_low);
    debug_print("\r\n");

    if (cmd==RFS_SETTINGS)  {
        switch (arg_high) {
            case RFS_SET_START:
                if (!state.base.spectrometer_enable) {
                    RFS_start();
                    flash_store_pointer = heartbeat_counter%MAX_STATE_SLOTS;
                    flash_state_store(flash_store_pointer);
                }
                break;
            case RFS_SET_STOP:
                if (state.base.spectrometer_enable) {
                    flash_state_clear(flash_store_pointer);
                    RFS_stop();
                }
                break;
            case RFS_SET_RESET:
                RFS_stop();
                spec_set_reset();
                // arglow controls what to do with the stored states after reset.
                spec_write_uC_register(0,arg_low);
                soft_reset_flag = true;
                return true;
            case RFS_SET_STORE:
                spec_store();
                break;
            case RFS_SET_RECALL:
                spec_recall();
                break;
            case RFS_SET_HK_REQ:
                if ((arg_low < 2) || (arg_low == 99)) {
                    housekeeping_request = 1+arg_low;
                } else {
                    state.base.errors |= CDI_COMMAND_BAD_ARGS;
                }
                break;

            case RFS_SET_RANGE_ADC:
                range_adc = 1;
                trigger_ADC_stat();
                break;

            case RFS_SET_WAVEFORM:
                if (arg_low<8) request_waveform = arg_low | 8;
                else state.base.errors |= CDI_COMMAND_BAD_ARGS;
                break;

            case RFS_SET_DEBUG:
                debug_helper(arg_low);
                break;

            case RFS_SET_TIME_TO_DIE:
	        debug_print("Recevied time-to-die.\n\r");
                return true;

            case RFS_SET_LOAD_FL:
                // load the sequencer program # arg_low (0-255) into state.program
	      debug_print("Recevied RFS_SET_LOAD_FL.\n\r");
                cdi_not_implemented("RFS_SET_LOAD_FL");
                break;
            case RFS_SET_STORE_FL:
                // store the sequencer program # arg_low (0-255) from state.program into flash
                cdi_not_implemented("RFS_SET_STORE_FL");
                break;

            case RFS_SET_GAIN_ANA_SET:
                for (int i=0; i<NINPUT; i++){
                    uint8_t val = (arg_low >> (2*i)) & 0x03;
                    if (val==3) {
                        if  (state.seq.gain[i] != GAIN_DISABLE){
                            state.seq.gain[i] = GAIN_AUTO;
                        }
                    } else {
                       state.seq.gain[i] = val;
                        state.base.actual_gain[i] = val;
                    }
                }
                update_spec_gains();
                break;

            case RFS_SET_DISABLE_ADC:
                for (int i=0; i<NINPUT; i++){
                    if ((arg_low >> (i)) & 0x01) {
                        state.seq.gain[i] = GAIN_DISABLE;
                        state.base.actual_gain[i] = GAIN_DISABLE;
                    }
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
                state.base.actual_bitslice[xcor] = val;
                break;
            case RFS_SET_BITSLICE_HIGH:
                xcor = (arg_low & 0x07) + 8;
                val = (arg_low & 0xF8) >> 3;
                state.seq.bitslice[xcor] = val;
                state.base.actual_bitslice[xcor] = val;
                break;
            case RFS_SET_BITSLICE_AUTO:
                if (arg_low > 0) {
                    for (int i=0; i<NSPECTRA; i++) state.seq.bitslice[i] = 0xFF;
                    state.seq.bitslice_keep_bits = arg_low;
                } else {
                    for (int i=0; i<NSPECTRA; i++) state.seq.bitslice[i] = 0x1F;
                }
                break;

            case RFS_SET_ROUTE_SET1:
                set_route (0, arg_low);
                break;

            case RFS_SET_ROUTE_SET2:
                set_route (1, arg_low);
                break;

            case RFS_SET_ROUTE_SET3:
                set_route (2, arg_low);
                break;

            case RFS_SET_ROUTE_SET4:
                set_route (3, arg_low);
                break;

            case RFS_SET_ADC_SPECIAL:
                switch(arg_low) {
                    case 0:
                        spec_set_enable_digital_func(false);
                        spec_set_ADC_normal_ops();
                        break;
                    case 1:
                        spec_set_enable_digital_func(true);
                        spec_set_ADC_ramp();
                        break;
                    case 2:
                        spec_set_enable_digital_func(true);
                        spec_set_ADC_all_ones();
                        break;
                    case 3:
                        spec_set_enable_digital_func(true);
                        spec_set_ADC_all_zeros();
                        break;
                    default:
                        state.base.errors |= CDI_COMMAND_BAD_ARGS;
                        break;
                }
                 spec_set_ADC_ramp(arg_low);
                break;


            case RFS_SET_AVG_SET:
                state.seq.Navg1_shift = arg_low & 0x0F;
                state.seq.Navg2_shift = (arg_low & 0xF0) >> 4;

                fill_derived();
                break;
            case RFS_SET_AVG_FREQ:
                state.seq.Navgf = arg_low;
                fill_derived();
                break;
            case RFS_SET_AVG_NOTCH:
                state.seq.notch = arg_low;
                break;
            case RFS_SET_AVG_SET_HI:
                state.seq.hi_frac = arg_low;
                break;
            case RFS_SET_AVG_SET_MID:
                state.seq.med_frac = arg_low;
                break;
            case RFS_SET_OUTPUT_FORMAT:
                if (arg_low > 2) {
                    state.base.errors |= CDI_COMMAND_BAD_ARGS;
                } else {
                    state.seq.format = arg_low;
                }
                break;

            case RFS_SET_PRODMASK_LOW:
                state.base.corr_products_mask = (state.base.corr_products_mask & 0xFF00) | arg_low;
                break;
            case RFS_SET_PRODMASK_HIGH:
                state.base.corr_products_mask = (state.base.corr_products_mask & 0x00FF) | (arg_low << 8);
                break;

            case RFS_SET_REJ_SET:
                state.seq.reject_ratio = arg_low;
                break;
            case RFS_SET_REJ_NBAD:
                state.seq.reject_maxbad = arg_low;
                break;
            case RFS_SET_TR_START_LSB:
                state.seq.tr_start = ((state.seq.tr_start & 0xFF00) +arg_low);
                break;
            case RFS_SET_TR_STOP_LSB:
                state.seq.tr_stop = ((state.seq.tr_stop & 0xFF00) +arg_low);
                break;
            case RFS_SET_TR_ST_MSB:
                state.seq.tr_start = ((state.seq.tr_start & 0x00FF) + ((arg_low & (0x0F)) << 8));
                state.seq.tr_stop = ((state.seq.tr_stop & 0x00FF) + ((arg_low & (0xF0)) << 4));
                break;
            case RFS_SET_TR_AVG_SHIFT:
                state.seq.tr_avg_shift = arg_low;
                state.tr_avg = 1 << state.seq.tr_avg_shift;
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
                    state.program.sequencer_repeat = arg_low;
                }
                break;
            case RFS_SET_SEQ_CYC:
                if (state.base.spectrometer_enable) {
                    state.base.errors |= CDI_COMMAND_BAD;
                } else {
                    state.program.Nseq = arg_low;
                    state.base.sequencer_step = 0;
                }
                break;
            case RFS_SET_SEQ_STO:
                if (state.base.spectrometer_enable) {
                    state.base.errors |= CDI_COMMAND_BAD;
                } else {
                    state.program.seq[state.base.sequencer_step] = state.seq;
                    state.program.seq_times[state.base.sequencer_step] = arg_low;
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
