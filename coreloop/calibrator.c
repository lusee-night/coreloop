#include "core_loop.h"
#include "calibrator_interface.h"
#include "cdi_interface.h"
#include "lusee_appIds.h"
#include "LuSEE_IO.h"
#include <string.h>

#define CAL_MODE0_CHUNKSIZE (1024*sizeof(uint32_t))  
#define CAL_MODE0_DATASIZE (CAL_MODE0_CHUNKSIZE*5+1*sizeof(uint32_t)) // 5 chunks + 1 register
#define CAL_MODE0_PACKETSIZE (CAL_MODE0_CHUNKSIZE*2) 
#define CAL_MODE0_APPID_OFFSET 0

#define CAL_MODE1_CHUNKSIZE (2048*sizeof(uint32_t)) // REAL or IMage Size
#define CAL_MODE1_DATASIZE (CAL_MODE1_CHUNKSIZE*2*4)  // x 4 channels
#define CAL_MODE1_PACKETSIZE (CAL_MODE1_CHUNKSIZE)
#define CAL_MODE1_APPID_OFFSET 3


#define CAL_MODE3_CHUNKSIZE (1024*sizeof(uint32_t))
#define CAL_MODE3_DATASIZE (24*CAL_MODE3_CHUNKSIZE) // 24 channels
#define CAL_MODE3_PACKETSIZE (3*1024*sizeof(uint32_t)) 
#define CAL_MODE3_APPID_OFFSET 11
uint32_t register_scratch[CAL_NREGS];
int32_t hk;

void calibrator_default_state (struct calibrator_state* cal) {

    cal->mode = 0x10;
    cal->Navg2 = 1; // 64
    cal->Navg3 = 10; // 1024
    cal->drift_guard = 120;
    cal->drift_step = 50;
    cal->antenna_mask = 0b1111;
    cal->notch_index = 2;
    cal->SNRon = 5;
    cal->SNRoff = 3;
    cal->Nsettle = 5;
    cal->delta_drift_corA = 5;
    cal->delta_drift_corB = 10;
    cal->pfb_index = 0;
    cal->weight_ndx = 0;
}



void set_calibrator(struct calibrator_state* cal) {
    calib_set_Navg(cal->Navg2, cal->Navg3);
    calib_set_drift_guard(cal->drift_guard);
    calib_set_drift_step(cal->drift_step);
    calib_antenna_mask(cal->antenna_mask);
    calib_set_notch_index(cal->notch_index);
    calibrator_set_SNR(cal);
    calib_set_Nsettle(cal->Nsettle);
    calib_set_delta_drift_corA(cal->delta_drift_corA);
    calib_set_delta_drift_corB(cal->delta_drift_corB);
    calib_set_PFB_index(cal->pfb_index);
    memset((void *) CAL_BUF, 0, CAL_MODE0_DATASIZE);
    if (cal->mode <=4) {
        calib_set_readout_mode (cal->mode);
    } else if (cal->mode == CAL_MODE_ZOOM) {
        calib_set_readout_mode(CAL_MODE_RAW1);
    } else {
        calib_set_readout_mode(CAL_MODE_RAW3);
    }
    if ((cal->mode == CAL_MODE_SNR_SETTLE) || (cal->mode == CAL_MODE_BIT_SLICER_SETTLE)) {
        calib_set_SNR_lock_on(0xFFFFFF);
        if (cal->mode == CAL_MODE_BIT_SLICER_SETTLE) {
            calibrator_slice_init(cal);
        }
    }   
    
    calibrator_set_slices(cal);
    cal_clear_df_flag();
}



void calibrator_set_SNR(struct calibrator_state* cal) {
    calib_set_SNR_lock_on(cal->SNRon);
    calib_set_SNR_lock_off(cal->SNRoff);
}

void calibrator_slice_init(struct calibrator_state* cal) {
    cal->powertop_slice = 14;
    cal->sum1_slice = 14;
    cal->sum2_slice = 14;
    cal->prod1_slice = 14;
    cal->prod2_slice = 14;
}

void calibrator_set_slices(struct calibrator_state* cal) {
    calib_set_slicers(cal->powertop_slice, cal->powertop_slice+4, cal->sum1_slice, cal->sum2_slice, 0, cal->prod1_slice, cal->prod2_slice);
}


#define SLICER_ERR_SUM1 1
#define SLICER_ERR_SUM2 2
#define SLICER_ERR_FD   4
#define SLICER_ERR_SD1   8
#define SLICER_ERR_SD2   16
#define SLICER_ERR_SD3   32
#define SLICER_ERR_PTOP  64
#define SLICER_ERR_PBOT  128
#define SLICER_ERR_PROD1  256
#define SLICER_ERR_PROD2  512


struct calibrator_metadata* process_cal_mode11(struct core_state* state) {
    struct calibrator_metadata* out = (struct calibrator_metadata *)CAL_DATA;

    out->version = VERSION_ID;
    new_unique_packet_id(state);
    out->unique_packet_id = state->unique_packet_id;
    out->time_32 = state->base.time_32;
    out->time_16 = state->base.time_16;
    out->state = state->cal;
    out->SNR_max = 0;
    out->SNR_min = 0;
    uint32_t* have_lock = (uint32_t *)(CAL_BUF + 0*CAL_MODE3_CHUNKSIZE);
    out->have_lock[0] = out->have_lock[1] = out->have_lock[2] = out->have_lock[3] = 0;
    for (int i=0; i<1024; i++) {
        if (*have_lock & 1) out->have_lock[0]++;
        if (*have_lock & 2) out->have_lock[1]++;
        if (*have_lock & 4) out->have_lock[2]++;
        if (*have_lock & 8) out->have_lock[3]++;
        have_lock++;
    }
    // drift is chunk
    memcpy ((void*) &out->drift, (void *)CAL_BUF + 1*CAL_MODE3_CHUNKSIZE, CAL_MODE3_CHUNKSIZE);
    int32_t* SNR = (int32_t *)(CAL_BUF + 20*CAL_MODE3_CHUNKSIZE);
    out->SNR_max = 0;
    out->SNR_min = 0xFFFFFF;
    for (int i=0; i<4*1024; i++) {
        if (SNR[i] > out->SNR_max) out->SNR_max = SNR[i];
        if (SNR[i] < out->SNR_min) out->SNR_min = SNR[i];
    }

    //cal_copy_error_regs (&(out->error_regs));
    cal_clear_df_flag();
    state->cdi_dispatch.cal_count=0;
    state->cdi_dispatch.cal_packet_id = state->unique_packet_id;
    state->cdi_dispatch.cal_appId = AppID_Calibrator_MetaData;
    state->cdi_dispatch.cal_size = sizeof(struct calibrator_metadata);
    return out;
}

void process_cal_mode00(struct core_state* state) {
    memcpy ((void *) CAL_DATA, (void *) CAL_BUF,  CAL_MODE0_DATASIZE);
    cal_clear_df_flag();
    state->cdi_dispatch.cal_count=0;
    new_unique_packet_id(state);
    state->cdi_dispatch.cal_packet_id =state->unique_packet_id;
    state->cdi_dispatch.cal_appId = AppID_Calibrator_Data;
    state->cdi_dispatch.cal_size = CAL_MODE0_DATASIZE;
    state->cdi_dispatch.cal_packet_size = CAL_MODE0_PACKETSIZE;
}


void process_cal_mode01(struct core_state* state) {
    memcpy ((void *) CAL_DATA, (void *) CAL_BUF,  CAL_MODE1_DATASIZE);
    cal_clear_df_flag();
    state->cdi_dispatch.cal_count=0;
    new_unique_packet_id(state);
    state->cdi_dispatch.cal_packet_id =state->unique_packet_id;
    state->cdi_dispatch.cal_appId = AppID_Calibrator_RawPFB;
    state->cdi_dispatch.cal_size = CAL_MODE1_DATASIZE;
    state->cdi_dispatch.cal_packet_size = CAL_MODE1_PACKETSIZE;
}

void process_cal_mode_raw11(struct core_state* state) {
    memcpy ( (void *) CAL_DATA, (void *) CAL_BUF,  CAL_MODE3_DATASIZE);
    cal_clear_df_flag();
    state->cdi_dispatch.cal_count=0;
    new_unique_packet_id(state);
    state->cdi_dispatch.cal_packet_id =state->unique_packet_id;
    state->cdi_dispatch.cal_appId = AppID_Calibrator_Debug;
    state->cdi_dispatch.cal_size = CAL_MODE3_DATASIZE;
    state->cdi_dispatch.cal_packet_size = CAL_MODE3_PACKETSIZE;
}


void process_calibrator(struct core_state* state) {
    if (!state->base.calibrator_enable) return;
    
    bool new_data = cal_new_cal_ready();
    if (new_data) debug_print("C");
    int readout_mode = calib_get_readout_mode();
    uint32_t bit_slicer_flags = calib_get_slicer_errors();
    if ((bit_slicer_flags>0) & (state->cal.mode >= CAL_MODE_BIT_SLICER_SETTLE)) {
        debug_print("B");
        debug_print_dec(bit_slicer_flags);
        state->cal.mode = CAL_MODE_BIT_SLICER_SETTLE;
    }
    struct calibrator_state* cal = &(state->cal);
    if (cal->mode == CAL_MODE_BIT_SLICER_SETTLE) {
        if ((bit_slicer_flags == 0) & (new_data)) {
            // We have converged, time to move onto the next mode;
            cal->mode = CAL_MODE_SNR_SETTLE;
            debug_print("\r\n[ -> SNR]")
        } else {
            bool restart = true;
            if (bit_slicer_flags & SLICER_ERR_SUM1) { cal->sum1_slice+=2;  debug_print("\r\n[SUM1++]");}
            else if (bit_slicer_flags & SLICER_ERR_SUM2) { cal->sum2_slice+=2;  debug_print("\r\n[SUM2++]");}
            else if (bit_slicer_flags & (SLICER_ERR_FD+SLICER_ERR_SD1+SLICER_ERR_SD2+SLICER_ERR_SD3)) { cal->sum1_slice++; cal->sum2_slice++; debug_print("\r\n[SUMX++]");}
            else if (bit_slicer_flags & SLICER_ERR_PTOP) { cal->powertop_slice+=1; debug_print("\r\n[PTOP++]");}
            else if (bit_slicer_flags & SLICER_ERR_PBOT) { cal->powertop_slice+=1; debug_print("\r\n[PBOT++]");}
            else if (bit_slicer_flags & SLICER_ERR_PROD1) { cal->prod1_slice+=2; debug_print("\r\n[PROD1++]");}
            else if (bit_slicer_flags & SLICER_ERR_PROD2) { cal->prod2_slice+=2; debug_print("\r\n[PROD2++]");}
            else {restart = false;}
            if (restart) { calibrator_set_slices(cal); cal_reset(); }    
            // do not return, can go straight into settle. (check later)
        }   
    }
    
    
    if (cal->mode == CAL_MODE_SNR_SETTLE) {
        if (new_data) {
            //assert (readout_mode == 0b11);
            struct calibrator_metadata* out = process_cal_mode11 (state);
            if ((out->SNR_max/out->SNR_min)>5)  {
                int diff = out->SNR_max - out->SNR_min;
                cal->SNRon = out->SNR_min + diff*3/4;
                cal->SNRoff = out->SNR_min + diff/4;
                calibrator_set_SNR(cal);
                calib_set_readout_mode(CAL_MODE_RAW0);
                cal->readout_mode = CAL_MODE_RAW0;
                cal->mode = CAL_MODE_RUN;
                debug_print("\r\n[ -> RUN]")
                return; 
            }            
        }
    }

    if (cal->mode == CAL_MODE_RUN) {
        if (new_data) {
            if (state->cdi_dispatch.cal_count <0x20) {
                state->base.errors |= DF_CAL_DROPPED;
                debug_print("!");
                cal_clear_df_flag();
                return;
            }
            if (cal_df_dropped()) state->base.errors |= DF_CAL_DROPPED;
            if (readout_mode == CAL_MODE_RAW0) {
                process_cal_mode00(state);
                //calib_set_readout_mode(CAL_MODE_RAW3);
            } else {
                struct calibrator_metadata* out = process_cal_mode11(state);
                if (out->have_lock[0] + out->have_lock[1] + out->have_lock[2] + out->have_lock[3] == 0) {
                    // we have lost lock, let's go back to SNR acquisition
                    calib_set_SNR_lock_on(0xFFFFFF);
                    cal->mode = CAL_MODE_SNR_SETTLE;
                } else {
                    // we have lock, not let's take some real data
                    cal->readout_mode = 0;
                    calib_set_readout_mode(CAL_MODE_RAW0);
                }                
            }
            
        } else {
            // if not new data and not transferring, let's look at the other side
            if (0&(state->cdi_dispatch.cal_count>0x20)) {
                if (readout_mode == CAL_MODE_RAW0) {
                    calib_set_readout_mode(CAL_MODE_RAW3);
                    cal->readout_mode = CAL_MODE_RAW3;
                    
                } else  {
                    calib_set_readout_mode(CAL_MODE_RAW0);
                    cal->readout_mode = CAL_MODE_RAW0;
                }
            return;
            }
        }
    }

    if (cal->mode == CAL_MODE_RAW0) {
        if (state->cdi_dispatch.cal_count <0x20) {
            debug_print("!");
            cal_clear_df_flag();
            return;
        }
        if (cal_df_dropped()) state->base.errors |= DF_CAL_DROPPED;
        process_cal_mode00(state);
    }

    if (cal->mode == CAL_MODE_RAW3) {
        process_cal_mode_raw11(state);
    }

}


void dispatch_calibrator_data(struct core_state* state) {
    
    struct delayed_cdi_sending* d = &(state->cdi_dispatch);
    wait_for_cdi_ready();
    if (d->cal_appId == AppID_Calibrator_MetaData) {
        // data already packed as we need it
        memcpy ((void *)(TLM_BUF), (void *)(CAL_BUF), d->cal_size);
        cdi_dispatch_uC(&(state->cdi_stats),d->cal_appId, d->cal_size); // +12 for the header
        d->cal_count=0xFE; //we're done (+1 will make it go to 0xFF)
        debug_print("c#");
    } else {
        uint32_t* ptr  = (uint32_t *)(TLM_BUF);
        *ptr = state->cdi_dispatch.cal_packet_id; ptr++;
        *ptr = state->base.time_32; ptr++;
        *ptr = state->base.time_16; ptr++;
        
        uint32_t start = (state->cdi_dispatch.cal_count-1)*d->cal_packet_size;
        if (start+d->cal_packet_size >= d->cal_size) {
            d->cal_packet_size = d->cal_size-start;
            d->cal_count=0xFE; //we're done (+1 will make it go to 0xFF)
        }
        memcpy ((void *)(ptr), (void *)(CAL_BUF+start), d->cal_packet_size);
        cdi_dispatch_uC(&(state->cdi_stats),d->cal_appId+state->cdi_dispatch.cal_count, d->cal_packet_size+12); // +12 for the header        
        debug_print("c");
    } 
    
}