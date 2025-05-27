#include "core_loop.h"
#include "calibrator_interface.h"
#include "cdi_interface.h"
#include "lusee_appIds.h"
#include "LuSEE_IO.h"
#include "fft.h"
#include <string.h>

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
    cal->Nsettle = 5;
    cal->delta_drift_corA = 1;
    cal->delta_drift_corB = 1;
    cal->pfb_index = 0;
    cal->weight_ndx = 0;
    // the following slices are not touched by auto slicer
    cal->auto_slice = true;
    cal->delta_powerbot_slice = 2;
    cal->sd2_slice = 0;
    cal->fd_slice = 8;
    cal->zoom_ch1 = 0;
    cal->zoom_ch2 = 1;
    cal->ddrift_guard = 2500;
    cal->gphase_guard = 200000;
    cal->use_float_fft = true;
    cal->zoom_avg_idx = 0;
    cal->max_zoom_avg_iters_per_call = 4;
    cal->raw11_every = 0x00; // always 0xFF; // never
    cal->raw11_counter = 0;
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


    memset((void *)CAL_DF, 0, CAL_MODE0_DATASIZE);

    if ((cal->mode == CAL_MODE_SNR_SETTLE) || (cal->mode == CAL_MODE_BIT_SLICER_SETTLE))
    {
        calib_set_SNR_lock_on(0xFFFFFF);
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

static float sqf(float x) { return x * x; }
static int32_t sqi(int32_t x) { return x * x; }

void process_cal_mode_raw11(struct core_state *state)
{
    // now that we have the flag, transfer data over;
    cal_transfer_data(0b11);
    state->cal.errors = calib_get_errors();
    state->cal.bitslicer_errors = calib_get_slicer_errors();

    // first repack into 16 bytes
    uint32_t *have_lock = (uint32_t *)(CAL_DF);
    uint16_t *have_lock_tgt = (uint16_t *)(CAL_DATA);
    for (int i = 0; i < 1024; i++)
    {
        *have_lock_tgt = ((*have_lock) & 0xFF) | (((*have_lock) & 0xFF0000) >> 8);
        have_lock++;
        have_lock_tgt++;
    }
    void *lock_ant = (void *)CAL_DATA + 1024 * sizeof(uint16_t);
    cal_copy_errors((struct calibrator_errors *)lock_ant);

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

struct calibrator_metadata *process_cal_mode11(struct core_state *state)
{

    struct calibrator_metadata *out = (struct calibrator_metadata *)CAL_DATA;
    struct calibrator_state *cal = &(state->cal);
    cal_transfer_data(0b11);

    cal->errors = calib_get_errors();
    cal->bitslicer_errors = calib_get_slicer_errors();

    out->version = VERSION_ID;
    new_unique_packet_id(state);
    out->unique_packet_id = state->unique_packet_id;
    out->time_32 = state->base.time_32;
    out->time_16 = state->base.time_16;

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

    // drift is chunk
    memcpy((void *)&(out->drift), (void *)CAL_DF + 1 * CAL_MODE3_CHUNKSIZE, CAL_MODE3_CHUNKSIZE);

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

    get_mode11_minmax_unsigned(out->SNR_max, out->SNR_min, 20);
    get_mode11_minmax_unsigned(out->ptop_max, out->ptop_min, 2);
    get_mode11_minmax_unsigned(out->pbot_max, out->pbot_min, 6);
    get_mode11_minmax_signed(out->FD_max, out->FD_min, 10);
    get_mode11_minmax_signed(out->SD_max, out->SD_min, 14);

    cal_clear_df_flag();
    state->cdi_dispatch.cal_count = 0;
    state->cdi_dispatch.cal_packet_id = state->unique_packet_id;
    state->cdi_dispatch.cal_appId = AppID_Calibrator_MetaData;
    state->cdi_dispatch.cal_size = sizeof(struct calibrator_metadata);

    return out;
}

void process_cal_mode00(struct core_state *state)
{
    // now that we have the flag, transfer data over;
    cal_transfer_data(00);
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

void compute_fft_for_zoom(int fft_idx, const struct core_state *state,
                          int32_t *ch1_real, int32_t *ch1_imag,
                          int32_t *ch2_real, int32_t *ch2_imag)
{
    int fft_offset = fft_idx * FFT_SIZE;

    int32_t *fft_input_ch1_re = ch1_real + fft_offset;
    int32_t *fft_input_ch1_im = ch1_imag + fft_offset;
    int32_t *fft_input_ch2_re = ch2_real + fft_offset;
    int32_t *fft_input_ch2_im = ch2_imag + fft_offset;

    if (state->cal.use_float_fft)
    {

#ifdef LN_CORELOOP_FFT_TIMING
        timer_start();
#endif
        float *fft_output_ch1_re = (float *)(CAL_DATA) + fft_idx * FFT_SIZE;
        float *fft_output_ch1_im = fft_output_ch1_re + (state->cal.zoom_Nfft + fft_idx) * FFT_SIZE;
        float *fft_output_ch2_re = fft_output_ch1_im + (state->cal.zoom_Nfft + fft_idx) * FFT_SIZE;
        float *fft_output_ch2_im = fft_output_ch2_re + (state->cal.zoom_Nfft + fft_idx) * FFT_SIZE;

        fft_float(fft_input_ch1_re, fft_input_ch1_im, fft_output_ch1_re, fft_output_ch1_im);

#ifdef LN_CORELOOP_FFT_TIMING
        state->fft_time = timer_stop();
#endif

        fft_float(fft_input_ch2_re, fft_input_ch2_im, fft_output_ch2_re, fft_output_ch2_im);
    }
    else
    {

        int32_t *fft_output_ch1_re = (int32_t *)(CAL_DATA) + fft_idx * FFT_SIZE;
        int32_t *fft_output_ch1_im = fft_output_ch1_re + (state->cal.zoom_Nfft + fft_idx) * FFT_SIZE;
        int32_t *fft_output_ch2_re = fft_output_ch1_im + (state->cal.zoom_Nfft + fft_idx) * FFT_SIZE;
        int32_t *fft_output_ch2_im = fft_output_ch2_re + (state->cal.zoom_Nfft + fft_idx) * FFT_SIZE;

#ifdef LN_CORELOOP_FFT_TIMING
        timer_start();
#endif

        fft_int(fft_input_ch1_re, fft_input_ch1_im, fft_output_ch1_re, fft_output_ch1_im);

#ifdef LN_CORELOOP_FFT_TIMING
        state->fft_time = timer_stop();
#endif
        fft_int(fft_input_ch2_re, fft_input_ch2_im, fft_output_ch2_re, fft_output_ch2_im);
    }
}

void correlate_and_accumulate(const struct core_state *state, int fft_idx, void *to_send)
{
    if (state->cal.use_float_fft)
    {
        float *ch1_re = (float *)(CAL_DATA) + fft_idx * FFT_SIZE;
        float *ch1_im = ch1_re + (state->cal.zoom_Nfft + fft_idx) * FFT_SIZE;
        float *ch2_re = ch1_im + (state->cal.zoom_Nfft + fft_idx) * FFT_SIZE;
        float *ch2_im = ch2_re + (state->cal.zoom_Nfft + fft_idx) * FFT_SIZE;

        float *ch1_autocorr_float = (float *)(to_send) + fft_idx * FFT_SIZE;
        float *ch2_autocorr_float = ch1_autocorr_float + (state->cal.zoom_Nfft + fft_idx) * FFT_SIZE;
        float *ch1_2_corr_real_float = ch2_autocorr_float + (state->cal.zoom_Nfft + fft_idx) * FFT_SIZE;
        float *ch1_2_corr_imag_float = ch1_2_corr_real_float + (state->cal.zoom_Nfft + fft_idx) * FFT_SIZE;

        for (int i = 0; i < FFT_SIZE; i++)
        {
            if (state->cal.zoom_avg_idx == 0)
            {
                ch1_autocorr_float[i] = (sqf(ch1_re[i]) + sqf(ch1_im[i])) / state->cal.zoom_Navg;
                ch2_autocorr_float[i] = (sqf(ch2_re[i]) + sqf(ch2_im[i])) / state->cal.zoom_Navg;
                ch1_2_corr_real_float[i] = (ch1_re[i] * ch2_re[i] + ch1_im[i] * ch2_im[i]) / state->cal.zoom_Navg;
                ch1_2_corr_imag_float[i] = (-ch1_re[i] * ch2_im[i] + ch1_im[i] * ch2_re[i]) / state->cal.zoom_Navg;
            }
            else
            {
                ch1_autocorr_float[i] += (sqf(ch1_re[i]) + sqf(ch1_im[i])) / state->cal.zoom_Navg;
                ch2_autocorr_float[i] += (sqf(ch2_re[i]) + sqf(ch2_im[i])) / state->cal.zoom_Navg;
                ch1_2_corr_real_float[i] += (ch1_re[i] * ch2_re[i] + ch1_im[i] * ch2_im[i]) / state->cal.zoom_Navg;
                ch1_2_corr_imag_float[i] += (-ch1_re[i] * ch2_im[i] + ch1_im[i] * ch2_re[i]) / state->cal.zoom_Navg;
            }
        }
    }
    else
    {
        int32_t *ch1_re = (int32_t *)(CAL_DATA) + fft_idx * FFT_SIZE;
        int32_t *ch1_im = ch1_re + (state->cal.zoom_Nfft + fft_idx) * FFT_SIZE;
        int32_t *ch2_re = ch1_im + (state->cal.zoom_Nfft + fft_idx) * FFT_SIZE;
        int32_t *ch2_im = ch2_re + (state->cal.zoom_Nfft + fft_idx) * FFT_SIZE;

        int32_t *ch1_autocorr_int = (int32_t *)(to_send) + fft_idx * FFT_SIZE;
        int32_t *ch2_autocorr_int = ch1_autocorr_int + (state->cal.zoom_Nfft + fft_idx) * FFT_SIZE;
        int32_t *ch1_2_corr_real_int = ch2_autocorr_int + (state->cal.zoom_Nfft + fft_idx) * FFT_SIZE;
        int32_t *ch1_2_corr_imag_int = ch1_2_corr_real_int + (state->cal.zoom_Nfft + fft_idx) * FFT_SIZE;

        for (int i = 0; i < FFT_SIZE; i++)
        {
            if (state->cal.zoom_avg_idx == 0)
            {
                ch1_autocorr_int[i] = (sqi(ch1_re[i]) + sqi(ch1_im[i])) / state->cal.zoom_Navg;
                ch2_autocorr_int[i] = (sqi(ch2_re[i]) + sqi(ch2_im[i])) / state->cal.zoom_Navg;
                ch1_2_corr_real_int[i] = (ch1_re[i] * ch2_re[i] + ch1_im[i] * ch2_im[i]) / state->cal.zoom_Navg;
                ch1_2_corr_imag_int[i] = (-ch1_re[i] * ch2_im[i] + ch1_im[i] * ch2_re[i]) / state->cal.zoom_Navg;
            }
            else
            {
                ch1_autocorr_int[i] += (sqi(ch1_re[i]) + sqi(ch1_im[i])) / state->cal.zoom_Navg;
                ch2_autocorr_int[i] += (sqi(ch2_re[i]) + sqi(ch2_im[i])) / state->cal.zoom_Navg;
                ch1_2_corr_real_int[i] += (ch1_re[i] * ch2_re[i] + ch1_im[i] * ch2_im[i]) / state->cal.zoom_Navg;
                ch1_2_corr_imag_int[i] += (-ch1_re[i] * ch2_im[i] + ch1_im[i] * ch2_re[i]) / state->cal.zoom_Navg;
            }
        }
    }
}

void dispatch_cal_zoom(struct core_state *state, void *to_send)
{
    struct delayed_cdi_sending *d = &(state->cdi_dispatch);
    wait_for_cdi_ready();

    uint32_t old_appId = d->cal_appId;
    uint32_t old_cal_size = d->cal_size;

    d->cal_appId = AppID_ZoomSpectra;
    d->cal_size = 4 * sizeof(int32_t) * FFT_SIZE;

    memcpy((void *)TLM_BUF, to_send, d->cal_size);

    cdi_dispatch_uC(&(state->cdi_stats), d->cal_appId, d->cal_size);

    debug_print("z#");

    // restore previous cal_appId, cal_size
    d->cal_appId = old_appId;
    d->cal_size = old_cal_size;
}

void process_cal_zoom(struct core_state *state)
{
    void *to_send = (void *)CAL_DATA + 4 * FFT_SIZE * state->cal.zoom_Nfft * sizeof(int32_t);

    int n_iters = 0;

    if (state->cal.zoom_avg_idx < state->cal.zoom_Navg)
    {
        // after this function finishes, raw PFB outputs will be in CAL_DF
        cal_transfer_data(2);

        int32_t *ch1_real = (int32_t *)CAL_DF;
        int32_t *ch1_imag = ch1_real + NCHANNELS;
        int32_t *ch2_real = ch1_imag + NCHANNELS;
        int32_t *ch2_imag = ch2_real + NCHANNELS;

        for (int fft_idx = 0; fft_idx < state->cal.zoom_Nfft; ++fft_idx)
        {
            compute_fft_for_zoom(fft_idx, state, ch1_real, ch1_imag, ch2_real, ch2_imag);
        }

        // we no longer need data from CAL_DF, all zoom_Nfft FFTs are stored at CAL_DATA
        cal_clear_df_flag();

        for (int fft_idx = 0; fft_idx < state->cal.zoom_Nfft; ++fft_idx)
        {
            correlate_and_accumulate(state, fft_idx, to_send);
        }

        state->cal.zoom_avg_idx++;

        n_iters++;
        // TODO: replace max_zoom_avg_iters_per_call with #define-d constant?
        if (n_iters >= state->cal.max_zoom_avg_iters_per_call)
        {
            // if zoom_Navg iteration has passed, we go straight to sending next time process_cal_zoom is called
            return;
        }
    }

    if (state->cal.zoom_avg_idx == state->cal.zoom_Navg)
    {
        dispatch_cal_zoom(state, to_send);
        state->cal.zoom_avg_idx = 0;
    }
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

int check_range_signed(int32_t fields_max[4], uint32_t *fields_min, int32_t value, uint8_t antenna_mask)
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

void process_calibrator(struct core_state *state)
{

    static int32_t hk = 0;
    static int32_t old_errors = 0xFF;
    static int32_t old_bitslicer_errors = 0;
    struct calibrator_metadata *stats = NULL;

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
        debug_print("IMPLEMENT");
        if (df_ready[2])
        {
            process_cal_zoom(state);
        }
        return;
    }

    if (cal->mode == CAL_MODE_BIT_SLICER_SETTLE)
    {
        if (cal->auto_slice)
        {
            if (df_ready[3])
            {
                stats = process_cal_mode11(state);
                int ptop_shift = check_range_unsigned((uint32_t *)(stats->ptop_max), 1e7, cal->antenna_mask);
                int sd_shift = check_range_signed(stats->SD_max, stats->SD_min, 1e6, cal->antenna_mask);
                int fd_shift = check_range_signed(stats->FD_max, stats->FD_min, 1e7, cal->antenna_mask);
                debug_print ("Shift:");
                debug_print_dec(ptop_shift);
                debug_print(" ");
                debug_print_dec(sd_shift);
                debug_print(" ");
                debug_print_dec(fd_shift);
                debug_print("|");


                bool range_ok = ((ptop_shift == 0) && (sd_shift == 0) && (fd_shift == 0));

                if (range_ok)
                {
                    // We have converged, time to move onto the next mode;
                    cal->mode = CAL_MODE_SNR_SETTLE;
                    debug_print("\r\n[ -> SNR]")
                }
                else
                {

                    // if powertop changes, we need to adjust the slice, but wait with FD/SD since SD=sum0*sum2+sum1**2
                    if (ptop_shift > 0)
                    {
                        cal->powertop_slice = MAX(0, (int)(stats->powertop_slice) - ptop_shift);
                    }
                    else if (sd_shift > 0)
                    {
                        // powertop is ok. Let's increase sum1 and sum2 by half
                        int shift = (sd_shift+1)/2;
                        if (shift == 0 ) shift = 1;
                        cal->sum1_slice = MAX(0, (int)(stats->sum1_slice) - shift);
                        cal->sum2_slice = MAX(0, (int)(stats->sum2_slice) - shift);
                    }
                    else
                    {
                        // ok, the last one is FD
                        cal->fd_slice = MAX(0, (int)(stats->fd_slice) - fd_shift);
                    }

                    calibrator_set_slices(cal);
                    cal_reset();

                    debug_print("[SLICERS ");
                    debug_print_dec(cal->powertop_slice);
                    debug_print(" ");
                    debug_print_dec(cal->sum1_slice);
                    debug_print(" ");
                    debug_print_dec(cal->sum2_slice);
                    debug_print(" ");
                    debug_print_dec(cal->fd_slice);
                    debug_print(" ");
                    debug_print_dec(cal->prod1_slice);
                    debug_print(" ");
                    debug_print_dec(cal->prod2_slice);
                    debug_print(" ]\n\r");
                }
            }
        }
        else
        {
            // if not on auto slicer we can just move on
            cal->mode = CAL_MODE_SNR_SETTLE;
        }
    }

    if (cal->mode == CAL_MODE_SNR_SETTLE)
    {
        if (df_ready[3] || (stats))
        {
            if (!stats)
            stats = process_cal_mode11(state);
            
            uint32_t SNR_on = 0;
            
            for (int ant = 0; ant < 4; ant++) {
                if (!(cal->antenna_mask & (1 << ant)))
                {
                    continue;
                }
                
                float ratio = (stats->SNR_max[ant] / stats->SNR_min[ant]);
                int diff = stats->SNR_max[ant] - stats->SNR_min[ant];
                debug_print("here:");
                debug_print_dec (stats->SNR_max[ant]); debug_print (" "); debug_print_dec(stats->SNR_min[ant]);
                if (ratio < 2.5)
                {
                    // we do not have the calibrator around, let's set the bar just over
                    //cal->SNRon = stats->SNR_max[ant] + diff / 3;
                    //cal->SNRoff = 0;
                }
                else
                {
                    // ok, the signal is present, let's just set the bar a bit below max;
                    SNR_on = MAX (SNR_on, stats->SNR_min[ant] + diff * 3 / 4);                    
                }                
            }
            if (SNR_on>0) {
                cal->mode = CAL_MODE_RUN;
                debug_print("\r\n[ -> RUN]");
                calibrator_set_SNR(cal);
            }    
            return;                
        }
    }
    
    if (mode == CAL_MODE_RUN) {
        if (df_ready[0])
        {
            process_cal_mode00(state);
            return;
        }
        if (df_ready[3])
        {
            cal->raw11_counter++;
            if ((cal->raw11_every < 0xff) && (cal->raw11_counter >= cal->raw11_every))
            {
                cal->raw11_counter = 0;
                process_cal_mode_raw11(state);
            }
            else
            {
                process_cal_mode11(state);
                // now check prod1 and prod2 slices
                if (cal->auto_slice)
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
            process_cal_mode_raw11(state);
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
        memcpy((void *)(ptr), (void *)(CAL_DATA + start), d->cal_packet_size);
        cdi_dispatch_uC(&(state->cdi_stats), appid, d->cal_packet_size + 12); // +12 for the header
        debug_print("c");
    }
}
