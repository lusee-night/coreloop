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
}

void cmd_soft_reset(uint8_t arg_low, struct core_state* state)
{
    RFS_stop(state);
    spec_set_reset();
    // arglow controls what to do with the stored states after reset.
    spec_write_uC_register(0,arg_low & 0b011);
    spec_write_uC_register(1,arg_low & 0b100);
    state->soft_reset_flag = true;
}

bool process_cdi(struct core_state* state)
{
    uint8_t cmd, arg_high, arg_low;
    uint8_t ch, xcor, val;
    uint8_t ant1low, ant1high, ant2low, ant2high, ant3low, ant3high, ant4low, ant4high;
    uint32_t dly;

    #ifdef NOTREAL
    cdi_fill_command_buffer();
    #endif
    if (cdi_new_command(&cmd, &arg_high, &arg_low)) {
        // process incoming commands
        state->cmd_counter++;
        debug_print ("[>>");
        debug_print_hex(cmd);
        debug_print_hex(arg_high);
        debug_print_hex(arg_low);
        debug_print("]");

        if (cmd == RFS_SPECIAL) {
            switch (arg_high) {
            case RFS_SET_RESET:
                cmd_soft_reset(arg_low, state);
                return true;
            case RFS_SET_TIME_TO_DIE:
                return true;
            case RFS_SET_SEQ_BEGIN:
                state->sequence_upload = true;
                return false;
            case RFS_SET_SEQ_END:
                state->sequence_upload = false;
                // flash storing
                if (arg_low>0) store_state(state);
                return false;
            case RFS_SET_BREAK:
                // reset the commanding;
                state->cmd_ptr = state->cmd_end = 0;
                state->loop_depth = 0;
                return false;
            default:
                cdi_not_implemented("RFS_SPECIAL");
                state->base.errors |= CDI_COMMAND_UNKNOWN;
                return false;
            }
        } else if (cmd==RFS_SETTINGS)  {
            uint16_t cmd_end = (state->cmd_end + 1) % CMD_BUFFER_SIZE;
            if (cmd_end == state->cmd_ptr) {
                state->base.errors != CDI_COMMAND_BUFFER_OVERFLOW;
                cmd_end = state->cmd_end;
                return false;
            } else {
                state->cmd_arg_low[cmd_end] = arg_low;
                state->cmd_arg_high[cmd_end] = arg_high;
                state->cmd_end = cmd_end;
            }
        } else {
        debug_print ("   Commmand not implemented, ignoring.\n\r");
        state->base.errors |= CDI_COMMAND_UNKNOWN;
        }
    }


    if (state->timing.cdi_wait_counter>tap_counter) return false; //not taking any commands while in the CDI wait state
    if (state->cdi_wait_spectra>0) return false; // not taking any command while waitig for spectra.
    if (state->clear_buffers && (!delayed_cdi_dispatch_done(state)) ) return false; // not taking any commands while waiting buffers to clear
    state->clear_buffers = false; // now clear the request
    if (state->sequence_upload) return false; // not taking any commands while uploading sequence.
    if (state->cmd_ptr == state->cmd_end) return false; // no new commands

    // finally process the command in the line
    state->cmd_ptr = (state->cmd_ptr + 1) % CMD_BUFFER_SIZE;
    arg_low = state->cmd_arg_low[state->cmd_ptr];
    arg_high = state->cmd_arg_high[state->cmd_ptr];
    debug_print ("[>*");
    debug_print_hex(cmd);
    debug_print_hex(arg_high);
    debug_print_hex(arg_low);
    debug_print("]");

    switch (arg_high) {
        case RFS_SET_START:
            if (!state->base.spectrometer_enable) {
                RFS_start(state);
            }
            break;

        case RFS_SET_STOP:
            if (state->base.spectrometer_enable) {
                RFS_stop(state);
                state->clear_buffers = true;
            }
            break;

        case RFS_SET_RESET:
            cmd_soft_reset(arg_low, state);
            return true;

        case RFS_SET_TEMP_ALARM:
            state->watchdog.FPGA_max_temp = arg_low;
            break;

        case RFS_SET_WAIT_SPECTRA:
            state->cdi_wait_spectra = arg_low;
            break;

        case RFS_SET_HK_REQ:
            if ((arg_low < 4) || (arg_low == 99)) {
                state->housekeeping_request = 1+arg_low;
            } else {
                state->base.errors |= CDI_COMMAND_BAD_ARGS;
            }
            break;

        case RFS_SET_RANGE_ADC:
            state->range_adc = 1;
            trigger_ADC_stat();
            break;

        case RFS_SET_WAVEFORM:
            if (arg_low<8) state->request_waveform = arg_low | 8;
            else state->base.errors |= CDI_COMMAND_BAD_ARGS;
            break;

        case RFS_SET_WAIT_TICKS:
            state->timing.cdi_wait_counter = tap_counter + arg_low;
            break;

        case RFS_SET_WAIT_SECS:
            state->timing.cdi_wait_counter = tap_counter + arg_low * 100;
            break;

        case RFS_SET_WAIT_MINS:
            state->timing.cdi_wait_counter = tap_counter + arg_low * 100*60;
            break;

        case RFS_SET_WAIT_HRS:
            state->timing.cdi_wait_counter = tap_counter + arg_low * 100*60*60;
            break;

        case RFS_SET_DEBUG:
            debug_helper(arg_low, state);
            break;

        case RFS_SET_HEARTBEAT:
            if (arg_low == 0) {
                state->timing.heartbeat_counter = 0xFFFFFFFFFFFFFFFF;
            } else {
                state->timing.heartbeat_counter = tap_counter + HEARTBEAT_DELAY;
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
            state->dispatch_delay = arg_low;
            break;


        case RFS_SET_WR_ADR_LSB:
            state->reg_address = arg_low;
            state->reg_value = 0;
            break;

        case RFS_SET_WR_ADR_MSB:
            state->reg_address = (state->reg_address & 0xFF) | (arg_low << 8);
            break;

        case RFS_SET_WR_VAL_0:
            state->reg_value = (state->reg_value & 0xFFFFFF00) + arg_low;
            break;

        case RFS_SET_WR_VAL_1:
            state->reg_value = (state->reg_value & 0xFFFF00FF) + (arg_low << 8);
            break;

        case RFS_SET_WR_VAL_2:
            state->reg_value = (state->reg_value & 0xFF00FFFF) + (arg_low << 16);
            break;

        case RFS_SET_WR_VAL_3:
            state->reg_value = (state->reg_value & 0x00FFFFFF) + (arg_low << 24);
            spec_reg_write(state->reg_address, state->reg_value);
            break;

        case RFS_SET_ENABLE_WATCHDOGS:
            state->watchdog.watchdogs_enabled = arg_low;
            spec_enable_watchdogs(arg_low);
            break;

        case RFS_SET_TEST_WATCHDOG:
            switch (arg_low) {
                case 0x13:
                    state->watchdog.feed_uc = false;  // stop feeding uC watchdog
                    state->timing.heartbeat_counter = 0xFFFFFFFFFFFFFFFF;
                    break;
                case 0x49:
                    state->watchdog.tripped_mask |= (1 << 0);  // simulate another WD (bit 0 = ADC)
                    break;
                default:
                    state->base.errors |= CDI_COMMAND_BAD_ARGS;  // standard bad argument error
                    break;
            }
            break;

        case RFS_SET_EMPTY_BUFFERS:
            if (!state->base.spectrometer_enable)
                state->clear_buffers = true;
            else
                state->base.errors |= CDI_COMMAND_BAD_ARGS;  // can only be done when spectrometer is stopped
            break;

        case RFS_SET_LOOP_START:
            if (state->loop_depth < MAX_LOOPS) {
                state->loop_start[state->loop_depth] = state->cmd_ptr; //already pointing at the next cmd >0 ? state->cmd_ptr-1 : CMD_BUFFER_SIZE-1;
                state->loop_count[state->loop_depth] = arg_low;
                state->loop_depth++;
            } else {
                state->base.errors |= CDI_COMMAND_BAD_ARGS;
            }
            break;

        case RFS_SET_LOOP_NEXT:
            if (state->loop_depth > 0) {
                uint8_t loop_count = state->loop_count[state->loop_depth-1];
                if (loop_count ==0 ) { // infinite loops
                    state->cmd_ptr = state->loop_start[state->loop_depth-1];
                } else if (loop_count > 1) {
                    // decrease the counter and cycle back
                    state->loop_count[state->loop_depth-1]--;
                    state->cmd_ptr = state->loop_start[state->loop_depth-1];
                } else {
                    // this is the last iteration
                    state->loop_depth--;
                    // and let's move on
                }
            } else {
                state->base.errors |= CDI_COMMAND_BAD_ARGS;
            }
            break;

        case RFS_SET_SEQ_OVER:
            state->request_eos = arg_low;
            break;

        case RFS_SET_FLASH_CLR:
            clear_current_slot(state);
            break;

        case RFS_SET_GAIN_ANA_SET:
            for (int i=0; i<NINPUT; i++){
                uint8_t val = (arg_low >> (2*i)) & 0x03;
                if (val==3) {
                    if  (state->base.gain[i] != GAIN_DISABLE){
                        state->base.gain[i] = GAIN_AUTO;
                    }
                } else {
                    state->base.gain[i] = val;
                    state->base.actual_gain[i] = val;
                }
            }
            update_spec_gains(state);
            break;

        case RFS_SET_GAIN_ADOPT:
            for (int i=0; i<NINPUT; i++) {
                if ((arg_low & (1<<i)) && (state->base.gain[i] == GAIN_AUTO)) {
                    state->base.gain[i] = state->base.actual_gain[i];
                }
            }
            break;

        case RFS_SET_DISABLE_ADC:
            for (int i=0; i<NINPUT; i++){
                if ((arg_low >> (i)) & 0x01) {
                    state->base.gain[i] = GAIN_DISABLE;
                    state->base.actual_gain[i] = GAIN_DISABLE;
                }
            }
            update_spec_gains(state);
            break;

        case RFS_SET_GAIN_ANA_CFG_MIN:
            ch = arg_low & 0x03;
            val = (arg_low & 0xFC) >> 2;
            state->base.gain_auto_min[ch] = 16*val; //max 16*64 = 1024, which is 1/8th
            break;

        case RFS_SET_GAIN_ANA_CFG_MULT:
            ch = arg_low & 0x03;
            val = (arg_low & 0xFC) >> 2;
            state->base.gain_auto_mult[ch] = val;
            break;

        case RFS_SET_BITSLICE_LOW:
            xcor = arg_low & 0x07;
            val = (arg_low & 0xF8) >> 3;
            state->base.bitslice[xcor] = val;
            state->base.actual_bitslice[xcor] = val;
            break;

        case RFS_SET_BITSLICE_HIGH:
            xcor = (arg_low & 0x07) + 8;
            val = (arg_low & 0xF8) >> 3;
            state->base.bitslice[xcor] = val;
            state->base.actual_bitslice[xcor] = val;
            break;

        case RFS_SET_BITSLICE_AUTO:
            if (arg_low > 0) {
                for (int i=0; i<NSPECTRA; i++) state->base.bitslice[i] = 0xFF;
                state->base.bitslice_keep_bits = arg_low;
            } else {
                for (int i=0; i<NSPECTRA; i++) state->base.bitslice[i] = 0x1F;
            }
            break;

        case RFS_SET_ROUTE_SET1:
            set_route (state, 0, arg_low);
            break;

        case RFS_SET_ROUTE_SET2:
            set_route (state, 1, arg_low);
            break;

        case RFS_SET_ROUTE_SET3:
            set_route (state, 2, arg_low);
            break;

        case RFS_SET_ROUTE_SET4:
            set_route (state, 3, arg_low);
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
                    state->base.errors |= CDI_COMMAND_BAD_ARGS;
                    break;
            }
            break;


        case RFS_SET_AVG_SET:
            if (state->base.spectrometer_enable) {
                // changing settings while spectrometer is running is not allowed;
                // do nothing but set the error flag
                state->base.errors |= CDI_COMMAND_BAD;
            } else {
                state->base.Navg1_shift = 8 + (arg_low & 0x0F);
                state->base.Navg2_shift = (arg_low & 0xF0) >> 4;
            }
            break;

        case RFS_SET_AVG_FREQ:
            if (state->base.spectrometer_enable) {
                // changing settings while spectrometer is running is not allowed;
                // do nothing but set the error flag
                state->base.errors |= CDI_COMMAND_BAD;
            } else {
                state->base.Navgf = arg_low;
            }
            break;

        case RFS_SET_AVG_NOTCH:
            state->base.notch = arg_low;
            break;

        case RFS_SET_NOTCH_DETECTOR:
            if (arg_low > 0) {
                state->base.notch |= (1 << 5);
            } else {
                state->base.notch &= ~(1 << 5);
            }
            break;

        case RFS_SET_AVG_SET_HI:
            state->base.hi_frac = arg_low;
            break;

        case RFS_SET_AVG_SET_MID:
            state->base.med_frac = arg_low;
            break;

        case RFS_SET_OUTPUT_FORMAT:
            if (arg_low > (uint8_t)OUTPUT_16BIT_SHARED_LZ) {
                state->base.errors |= CDI_COMMAND_BAD_ARGS;
            } else {
                state->base.format = arg_low;
            }
            break;

        case RFS_SET_PRODMASK_LOW:
            state->base.corr_products_mask = (state->base.corr_products_mask & 0xFF00) | arg_low;
            break;

        case RFS_SET_PRODMASK_HIGH:
            state->base.corr_products_mask = (state->base.corr_products_mask & 0x00FF) | (arg_low << 8);
            break;

        case RFS_SET_REJ_SET:
            state->base.reject_ratio = arg_low;
            break;

        case RFS_SET_REJ_NBAD:
            state->base.reject_maxbad = arg_low;
            break;

        case RFS_SET_TR_START_LSB:
            if (state->base.spectrometer_enable) {
                // changing settings while spectrometer is running is not allowed;
                // do nothing but set the error flag
                state->base.errors |= CDI_COMMAND_BAD;
            } else {
                state->base.tr_start = ((state->base.tr_start & 0xFF00) +arg_low);
            }
            break;

        case RFS_SET_TR_STOP_LSB:
            if (state->base.spectrometer_enable) {
                state->base.errors |= CDI_COMMAND_BAD;
            } else {
                state->base.tr_stop = ((state->base.tr_stop & 0xFF00) +arg_low);
            }
            break;

        case RFS_SET_TR_ST_MSB:
            if (state->base.spectrometer_enable) {
                state->base.errors |= CDI_COMMAND_BAD;
            } else {
                state->base.tr_start = ((state->base.tr_start & 0x00FF) + ((arg_low & (0x0F)) << 8));
                state->base.tr_stop = ((state->base.tr_stop & 0x00FF) + ((arg_low & (0xF0)) << 4));
            }
            break;

        case RFS_SET_TR_AVG_SHIFT:
            if (state->base.spectrometer_enable) {
                state->base.errors |= CDI_COMMAND_BAD;
            } else {
                state->base.tr_avg_shift = arg_low;
            }
            break;

        case RFS_SET_GRIMMS_TALES:            
            state->base.grimm_enable = arg_low;
            break;

        case RFS_SET_GRIMM_W_NDX:
            if (arg_low>NGRIMM_WEIGHTS)
                state->base.errors |= CDI_COMMAND_BAD_ARGS;
            else
                state->grimm_weight_ndx = arg_low;
            break;

        case RFS_SET_GRIMM_W_VAL:
            if (arg_low<0xFF)
                state->grimm_weights[state->grimm_weight_ndx] = arg_low;
            else
                state->grimm_weights[state->grimm_weight_ndx] = 0x100;
            break;
            
        // CALIBRATOR SECTION
        case RFS_SET_CAL_ENABLE:
            if (arg_low<0xFF) {
                state->cal.mode = arg_low;
                state->base.calibrator_enable = true;
            } else {
                state->base.calibrator_enable = false;
            }
            break;

        case RFS_SET_CAL_AVG:
            state->cal.Navg2 = arg_low & 0x03;
            state->cal.Navg3 = (arg_low & 0x3C) >> 2;
            //calib_set_Navg(arg_low & 0x03, (arg_low & 0x3C) >> 2);
            break;

        case RFS_SET_CAL_NINDEX:
            state->cal.notch_index = arg_low;
            break;

        case RFS_SET_CAL_DRIFT_GUARD:
            state->cal.drift_guard = arg_low*20;            
            break;

        case RFS_SET_CAL_DRIFT_STEP:
            state->cal.drift_step = arg_low;
            break;

        case RFS_SET_CAL_ANT_EN:
            state->cal.antenna_mask = arg_low;
            break;

        case RFS_SET_CAL_SNR_ON:
            state->cal.SNRon = arg_low;
            break;

        case RFS_SET_CAL_SNR_ON_HIGH:
            state->cal.SNRon += (arg_low<<8);
            break;

        case RFS_SET_CAL_SNR_OFF:
            state->cal.SNRoff = arg_low;
            break;

        case RFS_SET_CAL_NSETTLE:
            state->cal.Nsettle = arg_low;
            break;

        case RFS_SET_CAL_CORRA_LSB:
            state->cal.delta_drift_corA = arg_low;
            break;

            case RFS_SET_CAL_CORRA_MSB:
            state->cal.delta_drift_corA += (arg_low << 8);
            break;

        case RFS_SET_CAL_CORRB_LSB:
            state->cal.delta_drift_corB = arg_low;
            break;

        case RFS_SET_CAL_CORRB_MSB:
            state->cal.delta_drift_corB += (arg_low << 8);
            break;

        case RFS_SET_CAL_WEIGHT_NDX_LO:
            state->cal.weight_ndx = arg_low;
            break;

        case RFS_SET_CAL_WEIGHT_NDX_HI:
            state->cal.weight_ndx = 256 + arg_low;
            break;

        case RFS_SET_CAL_WEIGHT_VAL:
            // this prevents some resolution loss, but mostly compulsive obsessive disorder
            // and does not seem to work either
            //if (arg_low == 0xff)
            //   calib_set_weight(state->cal.weight_ndx, 0x100);
            //else                     
            calib_set_weight(state->cal.weight_ndx, arg_low);
            state->cal.weight_ndx++;
            break;

        case RFS_SET_CAL_WEIGHT_ZERO:
            calib_zero_weights();
            break;

        case RFS_SET_CAL_PFB_NDX_LO:
            state->cal.pfb_index = arg_low + (state->cal.pfb_index & 0xFF00);
            calib_set_PFB_index(state->cal.pfb_index);
            state->cal.zoom_avg_idx = -1; 
            break;

        case RFS_SET_CAL_PFB_NDX_HI:
            state->cal.pfb_index = ((arg_low & 0x07) << 8) + (state->cal.pfb_index & 0x00FF);
            calib_set_PFB_index(state->cal.pfb_index);
            state->cal.zoom_avg_idx = -1; 
            break;

        case RFS_SET_CAL_BITSLICE: {
            int reg = arg_low >> 5;
            int val = arg_low & 0b00011111;
            if (reg==0) {
                state->cal.fd_slice = val;
            } else if (reg==1) {
                state->cal.powertop_slice = val;
            } else if (reg==2) {
                state->cal.sum1_slice = val;
            } else if (reg==3) {
                state->cal.sum2_slice = val;
            } else if (reg==4) {
                state->cal.prod1_slice = val;
            } else if (reg==5) {
                state->cal.prod2_slice = val;
            } else if (reg==6) {
                state->cal.delta_powerbot_slice = val;
            } else if (reg==7) {
                state->cal.sd2_slice = val;
            } else {
                // we should never end up here.
                state->base.errors |= INTERNAL_ERROR;
            }
            }
            break;

        case RFS_SET_CAL_BITSLICE_AUTO: 
            state->cal.auto_slice = (arg_low>0);
            break;

        case RFS_SET_CAL_DDRIFT_GUARD:
            state->cal.ddrift_guard = arg_low*25;
            break;

        case RFS_SET_CAL_GPHASE_GUARD:
            state->cal.gphase_guard = arg_low*2000;
            break;

        case RFS_SET_CAL_WSAVE:
            if (arg_low<16) {
                flash_calweights_store(arg_low);
                state->housekeeping_request = 1+HK_REQUEST_CAL_WEIGHT_CRC;
            } else {
                state->base.errors |= CDI_COMMAND_BAD_ARGS;
            }
            break;

        case RFS_SET_CAL_WLOAD:
            if (arg_low<16) {
                if (!flash_calweights_restore(arg_low, false)) {
                    state->base.errors |= FLASH_CRC_FAIL;
                }
            } else {
                state->base.errors |= CDI_COMMAND_BAD_ARGS;
            }
            break;
        
        case RFS_SET_CAL_SNR_RATIO:
            state->cal.SNR_minratio = arg_low;
            break;

        case RFS_SET_CAL_RAW11_EVERY:
            state->cal.raw11_every = arg_low;
            break;

        case RFS_SET_ZOOM_CH:
            state->cal.zoom_ch1 = arg_low & 0b0011;
            state->cal.zoom_ch2 = (arg_low & 0b1100) >> 2;
            state->cal.zoom_ch1_minus = (arg_low & 0b00110000) >> 4;
            state->cal.zoom_ch2_minus = (arg_low & 0b11000000) >> 6;
            break;

        case RFS_SET_ZOOM_NAVG:
            state->cal.zoom_Navg = (1 << arg_low);
            break;

        case RFS_SET_ZOOM_RANGE:
            state->cal.zoom_ndx_range = MIN(arg_low + (arg_low*arg_low)/32, 2048); 
            break;
        
        case RFS_SET_ZOOM_DIFF:
            state->cal.zoom_diff_1 = (arg_low & 0b01) != 0;
            state->cal.zoom_diff_2 = (arg_low & 0b10) != 0;
            break;


        case RFS_SET_AVG_MODE:
            if (arg_low > (uint8_t)AVG_FLOAT) {
                state->base.errors |= CDI_COMMAND_BAD_ARGS;
            } else {
                state->base.averaging_mode = arg_low;
            }
            break;

        case RFS_SET_REGION_UNLOCK:
            state->region_have_lock = (arg_low == 0xAB);
            break;

        case RFS_SET_REGION_INFO:
            if (state->region_have_lock) {
                flash_send_region_info(state); 
            } else {
                state->base.errors |= CDI_COMMAND_BAD_ARGS;
            }
            break;
        case RFS_SET_REGION_CPY:
            if (state->region_have_lock) {
                int region_src = arg_low & 0x0F;
                int region_tgt = (arg_low & 0xF0) >> 4;
                flash_copy_region_cmd(state, region_src, region_tgt);
            } else {
                state->base.errors |= CDI_COMMAND_BAD_ARGS;
            }
            break;
        case RFS_SET_REGION_ENABLE:
            if ((state->region_have_lock) & (arg_low>=1) & (arg_low<=6)) {
                flash_region_enable(arg_low, true);
            } else {
                state->base.errors |= CDI_COMMAND_BAD_ARGS;
            }
            break;

        case RFS_SET_REGION_DISABLE:
            if ((state->region_have_lock) & (arg_low>=1) & (arg_low<=6)) {
                flash_region_enable(arg_low, false);
            } else {
                state->base.errors |= CDI_COMMAND_BAD_ARGS;
            }
            break;

        default:
            debug_print ("UNRECOGNIZED RFS_SET COMMAND\n\r");
            state->base.errors |= CDI_COMMAND_UNKNOWN;
            break;
    }
    return false;
}
