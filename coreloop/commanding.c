#include "lusee_commands.h"
#include "spectrometer_interface.h"
#include "calibrator_interface.h"
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

void cmd_soft_reset(uint8_t arg_low)
{
    RFS_stop();
    spec_set_reset();
    // arglow controls what to do with the stored states after reset.
    spec_write_uC_register(0,arg_low);
    soft_reset_flag = true;
}

bool process_cdi()
{
    uint8_t cmd, arg_high, arg_low;
    uint8_t ch, xcor, val;
    uint8_t ant1low, ant1high, ant2low, ant2high, ant3low, ant3high, ant4low, ant4high;
    uint32_t dly;

    #ifdef NOTREAL
    cdi_fill_command_buffer();
    #endif

    
    if (!cdi_new_command(&cmd, &arg_high, &arg_low)) {
        // if there are no new commands, then the buffer is empty and we should agree
        // on the total number of commands received.
        if (state.cmd_counter != cdi_command_count()) {
            state.base.errors |= CDI_COMMAND_LOST;
            state.cmd_counter = cdi_command_count();
        }
    } else {
        // process incoming commands 
        state.cmd_counter++;
        debug_print ("\r\nGot new CDI command: cmd = ");
        debug_print_hex(cmd);
        debug_print(", arg_hi = ");
        debug_print_hex(arg_high);
        debug_print(", arg_lo = ");
        debug_print_hex(arg_low);
        debug_print("  ");

        if (cmd == RFS_SPECIAL) {
            if (arg_high == RFS_SET_RESET) {
                cmd_soft_reset(arg_low);
                return true;
            } else if (arg_high == RFS_SET_TIME_TO_DIE) {
                return true;
            } else {
                cdi_not_implemented("Special command");
            }
            return false;
        } else if (cmd==RFS_SETTINGS)  {
            uint16_t cmd_end = (state.cmd_end + 1) % CMD_BUFFER_SIZE;
            if (cmd_end == state.cmd_start) {
                state.base.errors != CDI_COMMAND_BUFFER_OVERFLOW;
                cmd_end = state.cmd_end;
                return false;
            } else {
                state.cmd_arg_low[cmd_end] = arg_low;
                state.cmd_arg_high[cmd_end] = arg_high;
                state.cmd_end = cmd_end;
            }
        } else {
        debug_print ("   Commmand not implemented, ignoring.\n\r");
        state.base.errors |= CDI_COMMAND_UNKNOWN;
        }
    }


    if (cdi_wait_counter>tap_counter) return false; //not taking any commands while in the CDI wait state
    if (state.cmd_start == state.cmd_end) return false; // no new commands
 
    // finally process the command in the line
    state.cmd_start = (state.cmd_start + 1) % CMD_BUFFER_SIZE;
    arg_low = state.cmd_arg_low[state.cmd_start];
    arg_high = state.cmd_arg_high[state.cmd_start];
    debug_print ("\r\nProcessing CDI command: cmd = ");
    debug_print_hex(cmd);
    debug_print(", arg_hi = ");
    debug_print_hex(arg_high);
    debug_print(", arg_lo = ");
    debug_print_hex(arg_low);
    debug_print("\r\n");

    switch (arg_high) {
        case RFS_SET_START:
            if (!state.base.spectrometer_enable) {
                RFS_start();
                if (!(arg_low & 1)) {
                    flash_store_pointer = tap_counter%MAX_STATE_SLOTS;
                    flash_state_store(flash_store_pointer);
                } else {
                    debug_print ("Not storing flash state.\r\n");
                }
            }
            break;
        case RFS_SET_STOP:
            if (state.base.spectrometer_enable) {
                RFS_stop();
                if (!(arg_low & 1)) {
                    flash_state_clear(flash_store_pointer);
                }
            }
            break;
        case RFS_SET_RESET:
            cmd_soft_reset(arg_low);
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

        case RFS_SET_WAIT_TICKS:
            cdi_wait_counter = tap_counter + arg_low;
            break;

        case RFS_SET_WAIT_SECS:            
            cdi_wait_counter = tap_counter + arg_low * 100;
            break;

        case RFS_SET_WAIT_MINS:
            cdi_wait_counter = tap_counter + arg_low * 100*60;
            break;

        case RFS_SET_WAIT_HRS:
            cdi_wait_counter = tap_counter + arg_low * 100*60*60;
            break;

        case RFS_SET_DEBUG:
            debug_helper(arg_low);
            break;

        case RFS_SET_HEARTBEAT:
            if (arg_low == 0) {
                heartbeat_counter = 0xFFFFFFFFFFFFFFFF;
            } else {
                heartbeat_counter = tap_counter + HEARTBEAT_DELAY;
            }
            break;

        case RFS_SET_TIME_TO_DIE:
            return true;


        case RFS_SET_CDI_FW_DLY:
            // 2^17 /102.4e6 = 1.28ms
            if (arg_low == 0) dly=0x17; else dly = (arg_low<<16);
            debug_print("Setting CDI delay to ");
            debug_print_dec(dly);
            spec_set_fw_cdi_delay (dly);
            return false;
            break;

        case RFS_SET_CDI_SW_DLY:
            state.dispatch_delay = arg_low;
            break;


        case RFS_SET_WR_ADR_LSB:
            state.reg_address = arg_low;
            state.reg_value = 0;
            break;

        case RFS_SET_WR_ADR_MSB:
            state.reg_address = (state.reg_address & 0xFF) | (arg_low << 8);
            break;

        case RFS_SET_WR_VAL_0:
            state.reg_value = (state.reg_value & 0xFFFFFF00) + arg_low;
            break;

        case RFS_SET_WR_VAL_1:
            state.reg_value = (state.reg_value & 0xFFFF00FF) + (arg_low << 8);
            break;

        case RFS_SET_WR_VAL_2:
            state.reg_value = (state.reg_value & 0xFF00FFFF) + (arg_low << 16);
            break;

        case RFS_SET_WR_VAL_3:
            state.reg_value = (state.reg_value & 0x00FFFFFF) + (arg_low << 24);
            spec_reg_write(state.reg_address, state.reg_value);
            break;


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
            if (state.base.spectrometer_enable) {
                // changing settings while spectrometer is running is not allowed;
                // do nothing but set the error flag
                state.base.errors |= CDI_COMMAND_BAD;
            } else {
                state.seq.Navg1_shift = arg_low & 0x0F;
                state.seq.Navg2_shift = (arg_low & 0xF0) >> 4;
            }

            break;
        case RFS_SET_AVG_FREQ:
            if (state.base.spectrometer_enable) {
                // changing settings while spectrometer is running is not allowed;
                // do nothing but set the error flag
                state.base.errors |= CDI_COMMAND_BAD;
            } else {
                state.seq.Navgf = arg_low;
            }
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
            if (arg_low > (uint8_t)OUTPUT_16BIT_SHARED_LZ) {
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
            if (state.base.spectrometer_enable) {
                // changing settings while spectrometer is running is not allowed;
                // do nothing but set the error flag
                state.base.errors |= CDI_COMMAND_BAD;
            } else {
                state.seq.tr_start = ((state.seq.tr_start & 0xFF00) +arg_low);
            }
            break;
        case RFS_SET_TR_STOP_LSB:
            if (state.base.spectrometer_enable) {
                state.base.errors |= CDI_COMMAND_BAD;
            } else {
                state.seq.tr_stop = ((state.seq.tr_stop & 0xFF00) +arg_low);
            }
            break;
        case RFS_SET_TR_ST_MSB:
            if (state.base.spectrometer_enable) {
                state.base.errors |= CDI_COMMAND_BAD;
            } else {
                state.seq.tr_start = ((state.seq.tr_start & 0x00FF) + ((arg_low & (0x0F)) << 8));
                state.seq.tr_stop = ((state.seq.tr_stop & 0x00FF) + ((arg_low & (0xF0)) << 4));
            }
            break;
        case RFS_SET_TR_AVG_SHIFT:
            if (state.base.spectrometer_enable) {
                state.base.errors |= CDI_COMMAND_BAD;
            } else {
                state.seq.tr_avg_shift = arg_low;
            }
            break;

        // CALIBRATOR SECTION
        case RFS_SET_CAL_ENABLE:
            calib_set_mode(arg_low);
            calib_enable(arg_low & 1);
            break;
        case RFS_SET_CAL_AVG:
            calib_set_Navg(arg_low & 0x03, (arg_low & 0x3C) >> 2);
            break;
        case RFS_SET_CAL_DRIFT_GUARD:
            calib_set_drift_guard(arg_low);
            break;
        case RFS_SET_CAL_DRIFT_STEP:
            calib_set_drift_step(arg_low);
            break;
        case RFS_SET_CAL_ANT_EN:
            calib_antenna_mask(arg_low);
            break;
        case RFS_SET_CAL_SNR_ON:
            calib_set_SNR_lock_on(arg_low);
            break;

        case RFS_SET_CAL_SNR_OFF:
            calib_set_SNR_lock_off(arg_low);
            break;

        case RFS_SET_CAL_NSETTLE:
            calib_set_Nsettle(arg_low);
            break;

        case RFS_SET_CAL_CORRA:
            calib_set_delta_drift_corA(arg_low);
            break;

        case RFS_SET_CAL_CORRB:
            calib_set_delta_drift_corB(arg_low);
            break;

        case RFS_SET_CAL_WEIGHT_NDX_LO:
            state.cal.weight_ndx = arg_low;
            break;
        
        case RFS_SET_CAL_WEIGHT_NDX_HI:
            state.cal.weight_ndx = 256 + arg_low;
            break;
        
        case RFS_SET_CAL_WEIGHT_VAL:
            calib_set_weight(state.cal.weight_ndx, arg_low);
            state.cal.weight_ndx++;
            break;

        case RFS_SET_CAL_WEIGHT_ZERO:
            calib_zero_weights();
            break;

        case RFS_SET_CAL_PFB_NDX_LO: 
            calib_set_PFB_index(arg_low+(calib_get_PFB_index()&0xFF00));
            break;
        case RFS_SET_CAL_PFB_NDX_HI:
            calib_set_PFB_index(((arg_low & 0x07) << 8) 
                        +(calib_get_PFB_index()&0x00FF));
            break;



         // SEQUENCER SECTION
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
    return false;
}
