#include "core_loop.h"
#include "calibrator.h"
#include "calibrator_interface.h"
#include "cdi_interface.h"
#include "lusee_appIds.h"
#include "LuSEE_IO.h"
#include "fft.h"
#include <string.h>
#include <stdlib.h>

#define CAL_MODE0_CHUNKSIZE (1024 * sizeof(uint32_t))
#define CAL_MODE0_DATASIZE (CAL_MODE0_CHUNKSIZE * 5 + 1 * sizeof(uint32_t)) // 5 chunks + 1 register
#define CAL_MODE0_PACKETSIZE (CAL_MODE0_CHUNKSIZE * 2)
#define CAL_MODE0_APPID_OFFSET 0

#define CAL_MODE1_CHUNKSIZE (2048 * sizeof(uint32_t))    // REAL or IMage Size
#define CAL_MODE1_DATASIZE (CAL_MODE1_CHUNKSIZE * 2 * 4) // x 4 channels
#define CAL_MODE1_PACKETSIZE (CAL_MODE1_CHUNKSIZE)
#define CAL_MODE1_APPID_OFFSET 3

#define CAL_MODE3_CHUNKSIZE (1024 * sizeof(uint32_t))
#define CAL_MODE3_DATASIZE (24 * CAL_MODE3_CHUNKSIZE) // 24 channels
#define CAL_MODE3_PACKETSIZE (3 * 1024 * sizeof(uint32_t))
#define CAL_MODE3_NPACKETS  (24/3)
#define CAL_MODE3_APPID_OFFSET 11
uint32_t register_scratch[CAL_NREGS];

void calibrator_default_state(struct calibrator_state *cal)
{

    cal->mode = 0x10;
    cal->Navg2 = 1;  // 64
    cal->Navg3 = 10; // 1024
    cal->drift_guard = 120;
    cal->drift_step = 50;
    cal->antenna_mask = 0b1111;
    cal->notch_index = 2;
    cal->SNRon = 5;
    cal->SNRoff = 3;
    cal->SNR_minratio = 16; // 16/8 = 2.0
    cal->Nsettle = 5;
    cal->delta_drift_corA = 1;
    cal->delta_drift_corB = 1;
    cal->pfb_index = 0;
    cal->weight_ndx = 0;
    // the following slices are not touched by auto slicer
    cal->auto_slice = 1;
    cal->delta_powerbot_slice = 2;
    cal->sd2_slice = 0;
    cal->fd_slice = 8;
    cal->zoom_ch1 = 0;
    cal->zoom_ch2 = 1;
    cal->zoom_ch1_minus = 2;
    cal->zoom_ch2_minus = 3;
    cal->zoom_diff_1 = false;
    cal->zoom_diff_2 = false;
    cal->ddrift_guard = 2500;
    cal->gphase_guard = 200000;
    // cal->use_float_fft = true; // now a define
    cal->raw11_every = 20; // every 20 we do a full packet
    cal->raw11_counter = 0;
    cal->zoom_ndx_range = 0;
}

void set_calibrator(struct calibrator_state *cal)
{
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
    calib_set_ddrift_max(cal->ddrift_guard);
    calib_set_gphase_max(cal->gphase_guard);
    cal->zoom_avg_idx = 0;
    cal->zoom_ndx_current = 0;

    memset((void *)CAL_DF, 0, CAL_MODE0_DATASIZE);

    if (cal->mode == CAL_MODE_BIT_SLICER_SETTLE) {
        calib_set_Navg(0, cal->Navg3);
        cal->settle_count = 0;
    }

    if ((cal->mode == CAL_MODE_SNR_SETTLE) || (cal->mode == CAL_MODE_BIT_SLICER_SETTLE))
    {
        calib_set_SNR_lock_on(0xFFFFFF);
    }

    if (cal->mode == CAL_MODE_RAW3)
    {
        // we need to set the raw11_every to 0x01 so that we output every single one;
        cal->raw11_every = 0x01;
    }
    
    calibrator_set_slices(cal);
    cal_clear_df_flag();
}

void calibrator_set_SNR(struct calibrator_state *cal)
{
    calib_set_SNR_lock_on(cal->SNRon);
    calib_set_SNR_lock_off(cal->SNRoff);
}

void calibrator_set_slices(struct calibrator_state *cal)
{
    // second one was plus +4
    calib_set_slicers(cal->powertop_slice, cal->powertop_slice + cal->delta_powerbot_slice, cal->sum1_slice, cal->sum2_slice, cal->fd_slice, cal->sd2_slice, cal->prod1_slice, cal->prod2_slice);
}


void get_mode11_minmax_signed(int32_t *max, int32_t *min, int reg)
{

    int32_t cmin, cmax;
    int32_t *tgt = (int32_t *)(CAL_DF + reg * CAL_MODE3_CHUNKSIZE);

    for (int ant = 0; ant < 4; ant++)
    {
        cmin = INT32_MAX;
        cmax = INT32_MIN;
        for (int i = 0; i < 1024; i++)
        {
            int32_t val = *tgt;
            if (val > cmax)
                cmax = val;
            if (val < cmin)
                cmin = val;
            tgt++;
        }
        min[ant] = cmin;
        max[ant] = cmax;
    }
}

void get_mode11_minmax_unsigned(uint32_t *max, uint32_t *min, int reg)
{

    uint32_t cmin, cmax;
    uint32_t mmin[4], mmax[4];
    uint32_t *tgt = (uint32_t *)(CAL_DF + reg * CAL_MODE3_CHUNKSIZE);

    for (int ant = 0; ant < 4; ant++)
    {
        cmin = UINT32_MAX;
        cmax = 0;
        for (int i = 0; i < 1024; i++)
        {
            uint32_t val = *tgt;
            if (val > cmax)
                cmax = val;
            if (val < cmin)
                cmin = val;
            tgt++;
        }
        mmin[ant] = cmin;
        mmax[ant] = cmax;
    }

    // why we have to do it this way is not clear, seems some weird compiler bug;
    for (int ant = 0; ant < 4; ant++)
    {
        min[ant] = mmin[ant];
        max[ant] = mmax[ant];
    }
}

void get_mode11_positive_count(uint16_t *count, int reg)
{
    int32_t *tgt = (int32_t *)(CAL_DF + reg * CAL_MODE3_CHUNKSIZE);
    for (int ant = 0; ant < 4; ant++)
    {
        count[ant] = 0;
        for (int i = 0; i < 1024; i++)
        {
            int32_t val = *tgt;
            if (val > 0)
                count[ant]++;
            tgt++;
        }
    }
}

uint16_t get_mode11_lock_count()
{

    // at zero
    uint32_t *tgt = (uint32_t *)(CAL_DF + 0 * CAL_MODE3_CHUNKSIZE);
    uint16_t count = 0;
    for (int i = 0; i < 1024; i++)
    {
        uint32_t val = *tgt;
        if (val > 0)
            count++;
        tgt++;
    }
    return count;
}

void copy_cal_metadata(struct calibrator_metadata *out, struct core_state *state, struct calibrator_stats* stats)
{
    struct calibrator_state *cal = &(state->cal);
    out->version = VERSION_ID;
    out->time_32 = state->base.time_32;
    out->time_16 = state->base.time_16;
    out->mode = cal->mode;
    out->SNRon = cal->SNRon;
    out->SNRoff = cal->SNRoff;
    out->powertop_slice = cal->powertop_slice;
    out->sum1_slice = cal->sum1_slice;
    out->sum2_slice = cal->sum2_slice;
    out->fd_slice = cal->fd_slice;
    out->sd2_slice = cal->sd2_slice;
    out->prod1_slice = cal->prod1_slice;
    out->prod2_slice = cal->prod2_slice;
    out->errors = state->cal.errors;
    out->bitslicer_errors = state->cal.bitslicer_errors;

    // now copy drift over
    int32_t* drift = (int32_t *)(CAL_DF + 1 * CAL_MODE3_CHUNKSIZE);
    int leading_zeros = 32;
    uint8_t shift;
    for (int i=0; i<1024; i+=8) {
        int32_t val = drift[i];
        int lz = 32;
        if (val != 0) {
            uint32_t uval = (val < 0) ? ~((uint32_t)val) : (uint32_t)val;
            lz = __builtin_clz(uval);
        }
        if (lz < leading_zeros) leading_zeros = lz;
    }
    
    if (leading_zeros<17) {shift = 17 - leading_zeros;} else {shift = 0;}
    for (int i=0,j=0; i<1024; i+=8, j++)  {
        out->drift[j] = (int16_t)(drift[i]>>shift);
    }

    out->drift_shift = shift;

    // sume have_locks
    uint32_t *have_lock = (uint32_t *)(CAL_DF + 0 * CAL_MODE3_CHUNKSIZE);
    out->have_lock[0] = out->have_lock[1] = out->have_lock[2] = out->have_lock[3] = 0;
    for (int i = 0; i < 1024; i++)
    {
        if (*have_lock & 1)
            out->have_lock[0]++;
        if (*have_lock & 2)
            out->have_lock[1]++;
        if (*have_lock & 4)
            out->have_lock[2]++;
        if (*have_lock & 8)
            out->have_lock[3]++;
        have_lock++;
    }
    cal_copy_errors(&out->error_reg);
    
    out->stats = *stats;
}


void packetize_mode11_raw(struct core_state *state,  struct calibrator_stats* stats)
{

    // we treat the first chunk specially

    // first repack into 16 bytes. This will give 2k of free space
    uint32_t *have_lock = (uint32_t *)(CAL_DF);
    uint16_t *have_lock_tgt = (uint16_t *)(CAL_DATA);
    for (int i = 0; i < 1024; i++)
    {
        *have_lock_tgt = ((*have_lock) & 0xFF) | (((*have_lock) & 0xFF0000) >> 8);
        have_lock++;
        have_lock_tgt++;
    }
    // at +2k we put in the std cal metadata
    struct calibrator_metadata *meta = (struct calibrator_metadata *)((void *)CAL_DATA + 1024 * 2);
    copy_cal_metadata(meta, state, stats);
    meta->unique_packet_id = 0; // saved elsewhere

    // now copy rest of data from CAL_MODE3_CHUNKSIZE (+4k) onwards
    memcpy((void *)(CAL_DATA + CAL_MODE3_CHUNKSIZE), (void *)(CAL_DF + CAL_MODE3_CHUNKSIZE),
           CAL_MODE3_DATASIZE - CAL_MODE3_CHUNKSIZE);

    // nextmode //debug
    cal_clear_df_flag();
    state->cdi_dispatch.cal_count = 0;
    new_unique_packet_id(state);
    state->cdi_dispatch.cal_packet_id = state->unique_packet_id;
    state->cdi_dispatch.cal_appId = AppID_Calibrator_Debug;
    state->cdi_dispatch.cal_size = CAL_MODE3_DATASIZE;
    state->cdi_dispatch.cal_packet_size = CAL_MODE3_PACKETSIZE;
}


void packetize_mode11_processed(struct core_state *state, struct calibrator_stats* stats)
{

    struct calibrator_metadata *out = (struct calibrator_metadata *)CAL_DATA;
    struct calibrator_state *cal = &(state->cal);

    copy_cal_metadata(out, state, stats);
    new_unique_packet_id(state);
    out->unique_packet_id = state->unique_packet_id;
  
    cal_clear_df_flag();
    state->cdi_dispatch.cal_count = 0;
    state->cdi_dispatch.cal_packet_id = state->unique_packet_id;
    state->cdi_dispatch.cal_appId = AppID_Calibrator_MetaData;
    state->cdi_dispatch.cal_size = sizeof(struct calibrator_metadata);

}

void process_cal_mode11 (struct core_state *state, struct calibrator_stats *stats)
{
    struct calibrator_state *cal = &(state->cal);
    cal_transfer_data(0b11);
    cal->errors = calib_get_errors();
    cal->bitslicer_errors = calib_get_slicer_errors(cal->antenna_mask);
    
    // now get stats;
    get_mode11_minmax_unsigned(stats->SNR_max, stats->SNR_min, 20);
    get_mode11_minmax_unsigned(stats->ptop_max, stats->ptop_min, 2);
    get_mode11_minmax_unsigned(stats->pbot_max, stats->pbot_min, 6);
    get_mode11_minmax_signed(stats->FD_max, stats->FD_min, 10);
    get_mode11_minmax_signed(stats->SD_max, stats->SD_min, 14);
    get_mode11_positive_count(stats->SD_positive_count, 14);
    stats->lock_count = get_mode11_lock_count();

    cal->raw11_counter++;
    if ((cal->raw11_every < 0xff) && (cal->raw11_counter >= cal->raw11_every)) {
        cal->raw11_counter = 0;
        packetize_mode11_raw(state, stats);
    } else {
        packetize_mode11_processed(state, stats);
    }

}


void process_cal_mode00(struct core_state *state)
{
    // now that we have the flag, transfer data over;
    cal_transfer_data(00);

    uint32_t *gNacc = (uint32_t *)(CAL_DF + 512*8*sizeof(uint32_t));
    if (*gNacc< 1<<(state->cal.Navg3-1)) {
        // if less than half integration period has data, balk at it
        debug_print ("~");
        cal_clear_df_flag();
        return;
    }

    memcpy((void *)CAL_DATA, (void *)CAL_DF, CAL_MODE0_DATASIZE);
    cal_clear_df_flag();
    state->cdi_dispatch.cal_count = 0;
    new_unique_packet_id(state);
    state->cdi_dispatch.cal_packet_id = state->unique_packet_id;
    state->cdi_dispatch.cal_appId = AppID_Calibrator_Data;
    state->cdi_dispatch.cal_size = CAL_MODE0_DATASIZE;
    state->cdi_dispatch.cal_packet_size = CAL_MODE0_PACKETSIZE;
}

void process_cal_mode_01_10(struct core_state *state, int mode)
{
    cal_transfer_data(mode);
    memcpy((void *)CAL_DATA, (void *)CAL_DF, CAL_MODE1_DATASIZE);
    cal_clear_df_flag();
    state->cdi_dispatch.cal_count = 0;
    new_unique_packet_id(state);
    state->cdi_dispatch.cal_packet_id = state->unique_packet_id;
    state->cdi_dispatch.cal_appId = AppID_Calibrator_RawPFB;
    state->cdi_dispatch.cal_size = CAL_MODE1_DATASIZE;
    state->cdi_dispatch.cal_packet_size = CAL_MODE1_PACKETSIZE;
}

int check_range_unsigned(uint32_t *fields, uint32_t value, uint8_t antenna_mask)
{
    int maxshift = 0;
    for (int i = 0; i < 4; i++)
    {
        if (!(antenna_mask & (1 << i)))
            continue; // skip if antenna is not enabled
        uint32_t cval = fields[i];
        if (cval == 0)
            cval = 1;
        uint32_t shift = 0;
        while (cval < value)
        {
            cval = (cval << 1);
            shift++;
        }
        maxshift = MAX(maxshift, shift);
    }
    return maxshift;
}

int check_range_signed(int32_t fields_max[4], int32_t *fields_min, int32_t value, uint8_t antenna_mask)
{
    int maxshift = 0;
    for (int i = 0; i < 4; i++)
    {
        if (!(antenna_mask & (1 << i)))
            continue; // skip if antenna is not enabled
        // we use abs to get the max number of bits
        int32_t cval = MAX(abs(fields_max[i]), abs(fields_min[i]));
        if (cval == 0)
            cval = 1;
        int32_t shift = 0;
        while (cval < value)
        {
            cval = (cval << 1);
            shift++;
        }
        maxshift = MAX(maxshift, shift);
    }
    return maxshift;
}

void return_to_bitslicer_settle(struct calibrator_state *cal)
{
    cal->powertop_slice += 7;
    cal->sum1_slice += 5;
    cal->sum2_slice += 5;
    cal->fd_slice += 5;
    cal->mode = CAL_MODE_BIT_SLICER_SETTLE;
    cal->settle_count = 0;
    calib_set_Navg(0, cal->Navg3); // set to fast settling
    calib_set_SNR_lock_on(0xFFFFFF);
    calibrator_set_slices(cal);
    debug_print("\r\n[ -> SLICER SETTLE]");
    cal_reset();
}

void process_calibrator(struct core_state *state)
{

    static int32_t hk = 0;
    static int32_t old_errors = 0xFF;
    static int32_t old_bitslicer_errors = 0;
    struct calibrator_stats stats;

    // if we are not enabled, return
    if (!state->base.calibrator_enable)
        return;
    // if we are still transferring, return
    if (state->cdi_dispatch.cal_count < NCALPACKETS)
        return;

    struct calibrator_state *cal = &(state->cal);
    bool df_ready[4];
    cal_new_cal_ready(df_ready);
    int mode = cal->mode;

    // if readout mode is zoom, we just do a special processing
    if (mode == CAL_MODE_ZOOM)
    {
        if (df_ready[2])
        {
            process_cal_zoom(state);
        }
        return;
    }

    if (cal->mode == CAL_MODE_BIT_SLICER_SETTLE)
    {
        if (cal->auto_slice & AUTO_SLICE_SETTLE)
        {
            calib_set_Navg(0, cal->Navg3);
            if (df_ready[3])
            {
                process_cal_mode11(state, &stats);
                int ptop_shift = check_range_unsigned((uint32_t *)(stats.ptop_max), 1e7, cal->antenna_mask);
                int sd_shift = check_range_signed(stats.SD_max, stats.SD_min, 1e6, cal->antenna_mask);
                int fd_shift = check_range_signed(stats.FD_max, stats.FD_min, 1e7, cal->antenna_mask);
                debug_print ("Shift:");
                debug_print_dec(ptop_shift);
                debug_print(" ");
                debug_print_dec(sd_shift);
                debug_print(" ");
                debug_print_dec(fd_shift);
                debug_print("|");


                bool range_ok = ((ptop_shift == 0) && (sd_shift == 0) && (fd_shift == 0));

                if (range_ok || (cal->settle_count >= MAX_SETTLE_COUNT))
                {
                    // We have converged, time to move onto the next mode;
                    calib_set_Navg(cal->Navg2, cal->Navg3);
                    cal_reset();
                    cal->mode = CAL_MODE_SNR_SETTLE;
                    debug_print("\r\n[ -> SNR]")
                }
                else
                {
                    cal->settle_count++;
                    // if powertop changes, we need to adjust the slice, but wait with FD/SD since SD=sum0*sum2+sum1**2
                    if (ptop_shift > 0) cal->powertop_slice = MAX(0, (int)(cal->powertop_slice) - ptop_shift);
                    if (sd_shift > 0) {
                        // powertop is ok. Let's increase sum1 and sum2 by half
                        int shift = (sd_shift+1)/2;
                        if (shift == 0 ) shift = 1;
                        cal->sum1_slice = MAX(0, (int)(cal->sum1_slice) - shift);
                        cal->sum2_slice = MAX(0, (int)(cal->sum2_slice) - shift);
                    }
                    if ((ptop_shift==0) & (sd_shift==0)) {
                        // powertop and sum1/sum2 are ok, let's adjust FD
                        // we need to shift FD by fd_shift, but we need to check if it is not negative
                        // if it is negative, we just set it to 0
                        // if it is positive, we set it to fd_slice - fd_shift
                        // so that we can still have some margin for the next iteration
                        if (fd_shift < 0) {
                            cal->fd_slice = MAX(0, (int)(cal->fd_slice));
                        } else {
                        // ok, the last one is FD
                        cal->fd_slice = MAX(0, (int)(cal->fd_slice) - fd_shift);
                        }
                    }
                    calibrator_set_slices(cal);
                    cal_reset();


                    //powertop.sum1.sum2.prod1.prod2.delta_powerbot.fd_slice,sd2_slice
                    debug_print("[SLICERS ");
                    debug_print_dec(cal->powertop_slice);
                    debug_print(".");
                    debug_print_dec(cal->sum1_slice);
                    debug_print(".");
                    debug_print_dec(cal->sum2_slice);
                    debug_print(".");
                    debug_print_dec(cal->prod1_slice);
                    debug_print(".");
                    debug_print_dec(cal->prod2_slice);
                    debug_print(".0.");
                    debug_print_dec(cal->fd_slice);
                    debug_print(".0 ]\n\r");
                    
                }
            }
       }
        else
        {
            calib_set_Navg(cal->Navg2, cal->Navg3);
            // if not on auto slicer we can just move on            
            cal->mode = CAL_MODE_SNR_SETTLE;
        }
    } else if (cal->mode == CAL_MODE_SNR_SETTLE)
    {
        if (df_ready[3])
        {
            process_cal_mode11(state, &stats); // this allow for passing STATS directly from previous mode
            if (cal->auto_slice & AUTO_SLICE_SNR) {
                uint32_t bit_slicer_flags = state->cal.bitslicer_errors;
                // SUM2 on purpose missing here since it seems to trigger randomly
                if (bit_slicer_flags & (SLICER_ERR_SUM1 | SLICER_ERR_FD | SLICER_ERR_SD1 | SLICER_ERR_SD2 | SLICER_ERR_SD3 | SLICER_ERR_PTOP | SLICER_ERR_PBOT))
                {
                    
                    debug_print_dec(bit_slicer_flags);
                    return_to_bitslicer_settle(cal);
                    //debug_print("here");
                    //debug_print_dec(cal->mode);

                    return;
                }
            }   
            
            uint32_t SNR_on = 0;
            
            for (int ant = 0; ant < 4; ant++) {
                if (!(cal->antenna_mask & (1 << ant)))
                {
                    continue;
                }
                int ratio;
                if (stats.SNR_min[ant] == 0) {
                    ratio = (stats.SNR_max[ant] * 8 / 1);
                } else {
                    ratio = (stats.SNR_max[ant] * 8 / stats.SNR_min[ant]); // ratio is float(ratio)*8
                }                
                int diff = stats.SNR_max[ant] - stats.SNR_min[ant];                
                debug_print_dec(ant);
                debug_print(":");
                debug_print_dec (stats.SNR_max[ant]); debug_print (" "); debug_print_dec(stats.SNR_min[ant]);
                debug_print("; ");
                if (ratio < cal->SNR_minratio)
                {
                    // we do not have the calibrator around, let's set the bar just over
                    //cal->SNRon = stats.SNR_max[ant] + diff / 3;
                    //cal->SNRoff = 0;
                }
                else
                {
                    // ok, the signal is present, let's just set the bar a bit below max;
                    SNR_on =  MAX(SNR_on, stats.SNR_min[ant] + diff * 9 / 10);                    
                }                
            }
            if (SNR_on>0) {
                cal->mode = CAL_MODE_RUN;
                cal->SNRon = SNR_on;
                debug_print("\r\n[ -> RUN]");
                calibrator_set_SNR(cal);
            }    
            return;                
        }
    } else if (mode == CAL_MODE_RUN) {
        if (df_ready[0])
        {
            process_cal_mode00(state);
            return;
        }
        if (df_ready[3])
        {
            process_cal_mode11(state, &stats);
            // now check prod1 and prod2 slices
            if (cal->auto_slice & AUTO_SLICE_PROD)
            {
                uint32_t bit_slicer_flags = state->cal.bitslicer_errors;
                if (bit_slicer_flags & SLICER_ERR_PROD2)
                {
                    cal->prod2_slice += 1;
                    debug_print("\r\n[PROD2++]");
                }
                else if (bit_slicer_flags & SLICER_ERR_PROD1)
                {
                    cal->prod1_slice += 1;
                    debug_print("\r\n[PROD1++]");
                }
                calibrator_set_slices(cal);
            }
            if (cal->auto_slice & AUTO_SLICE_RUN)
            {
                uint32_t bit_slicer_flags = state->cal.bitslicer_errors;
                if (bit_slicer_flags & (SLICER_ERR_SUM1 | SLICER_ERR_FD | SLICER_ERR_SD1 | SLICER_ERR_SD2 | SLICER_ERR_SD3 | SLICER_ERR_PTOP | SLICER_ERR_PBOT))
                {
                    
                    debug_print_dec(bit_slicer_flags);
                    return_to_bitslicer_settle(cal);
                    //debug_print("here");
                    //debug_print_dec(cal->mode);

                    return;
                }
            }

            // now check if we are actually loosing lock; in steady state approximately half (512) are positive, other half negative
            // so 430 is about 5 sigma in binomial
            // this also only makes sense if we are lock, otherwise we might be transitioning towards lock
            // FW will not lock on positive SD.
            uint16_t *SD_pos = stats.SD_positive_count;
            
            
            debug_print_dec(stats.lock_count);
            debug_print(" ");
            for (int i = 0; i < 4; i++)
            {
                debug_print_dec(SD_pos[i]);
                debug_print(" ");
            } 

            // the request on lock_count should not be too stringent, since we are looking
            // for the lock in the first iteration.
            if (stats.lock_count < 100 || ((SD_pos[0] > 430) && (SD_pos[1] > 430) && (SD_pos[2] > 430) && (SD_pos[3] > 430)))
            {
                // we have lost the lock, let's go back to SNR settled mode
                for (int i = 0; i < 4; i++)
                {
                    if (!(cal->antenna_mask & (1 << i)))
                        continue; // skip if antenna is not enabled
                    debug_print("SD:");
                    debug_print_dec(i);
                    debug_print(":");
                    debug_print_dec(SD_pos[i]);
                    debug_print(" ");
                }
                debug_print("\r\n[ -> SNR]");
                calib_set_SNR_lock_on(0xFFFFFF);
                calib_set_SNR_lock_off(0xFFFFFF);
                cal->mode = CAL_MODE_SNR_SETTLE;
            }
            return;
        }
    }

    if (mode == CAL_MODE_RAW0)
    {
        if (df_ready[0])
        {
            process_cal_mode00(state);
            return;
        }
    }

    if ((mode == CAL_MODE_RAW1) || (mode == CAL_MODE_RAW2))
    {
        if (df_ready[mode])
        {
            process_cal_mode_01_10(state, mode);
            return;
        }
    }

    if (mode == CAL_MODE_RAW3)
    {
        if (df_ready[3])
        {
            process_cal_mode11(state, &stats);
            return;
        }
    }
}

void dispatch_calibrator_data(struct core_state *state)
{

    struct delayed_cdi_sending *d = &(state->cdi_dispatch);
    wait_for_cdi_ready();
    if (d->cal_appId == AppID_Calibrator_MetaData)
    {
        // data already packed as we need it
        memcpy((void *)(TLM_BUF), (void *)(CAL_DATA), d->cal_size);
        cdi_dispatch_uC(&(state->cdi_stats), d->cal_appId, d->cal_size); // +12 for the header
        d->cal_count = 0xFE;                                             // we're done (+1 will make it go to 0xFF)
        debug_print("c#");
    }
    else
    {
        uint32_t *ptr = (uint32_t *)(TLM_BUF);
        *ptr = state->cdi_dispatch.cal_packet_id;
        ptr++;
        *ptr = state->base.time_32;
        ptr++;
        *ptr = state->base.time_16;
        ptr++;

        uint32_t start = (state->cdi_dispatch.cal_count) * d->cal_packet_size;
        uint32_t appid = d->cal_appId + state->cdi_dispatch.cal_count;
        if (start + d->cal_packet_size >= d->cal_size)
        {
            d->cal_packet_size = d->cal_size - start;
            d->cal_count = 0xFE;
        }
        //debug_print_hex(appid); debug_print(" "); debug_print_hex(AppID_Calibrator_Debug); debug_print(" "); debug_print_hex((AppID_Calibrator_Data+ CAL_MODE3_NPACKETS)); debug_print(" | ");
        if ((appid>=AppID_Calibrator_Debug) & (appid< (AppID_Calibrator_Debug+ CAL_MODE3_NPACKETS))) {
            // remove rle econde for the time being
            size_t size = rle_encode((void *)(ptr), (void *)(CAL_DATA + start), d->cal_packet_size);
            // old way, next two lines
            //size_t size = d->cal_packet_size;
            //memcpy((void *)(ptr), (void *)(CAL_DATA + start), d->cal_packet_size);

            while (size%4>0) { size++; }
            cdi_dispatch_uC(&(state->cdi_stats), appid, size + 12); // +12 for the header
            debug_print("r");
        } else {
            memcpy((void *)(ptr), (void *)(CAL_DATA + start), d->cal_packet_size);
            cdi_dispatch_uC(&(state->cdi_stats), appid, d->cal_packet_size + 12); // +12 for the header
            debug_print("c");
        }
    }
}
