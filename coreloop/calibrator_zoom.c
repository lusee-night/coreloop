#include "core_loop.h"
#include "calibrator_interface.h"
#include "cdi_interface.h"
#include "lusee_appIds.h"
#include "LuSEE_IO.h"
#include "fft.h"
#include <string.h>
#include <stdlib.h>

static float sqf(float x) { return x * x; }
static int32_t sqi(int32_t x) { return x * x; }

void compute_fft_for_zoom_mult(int fft_batch_idx, const struct core_state* state,
                          int32_t* ch1_real, int32_t* ch1_imag,
                          int32_t* ch2_real, int32_t* ch2_imag)
{
    int fft_offset = fft_batch_idx * NUM_FFTS_IN_ONE_GO * FFT_SIZE;

    int32_t* fft_input_ch1_re = ch1_real + fft_offset;
    int32_t* fft_input_ch1_im = ch1_imag + fft_offset;
    int32_t* fft_input_ch2_re = ch2_real + fft_offset;
    int32_t* fft_input_ch2_im = ch2_imag + fft_offset;

    if (state->cal.use_float_fft) {

        float* fft_output_ch1_re = (float*)(CAL_DATA) + fft_batch_idx * NUM_FFTS_IN_ONE_GO * FFT_SIZE;
        float* fft_output_ch1_im = fft_output_ch1_re + NCHANNELS;
        float* fft_output_ch2_re = fft_output_ch1_im + NCHANNELS;
        float* fft_output_ch2_im = fft_output_ch2_re + NCHANNELS;

        fft_float_multiple(fft_input_ch1_re, fft_input_ch1_im, fft_output_ch1_re, fft_output_ch1_im);
        fft_float_multiple(fft_input_ch2_re, fft_input_ch2_im, fft_output_ch2_re, fft_output_ch2_im);
    } else {

        int32_t* fft_output_ch1_re = (int32_t*)(CAL_DATA) + fft_batch_idx * NUM_FFTS_IN_ONE_GO * FFT_SIZE;
        int32_t* fft_output_ch1_im = fft_output_ch1_re + NCHANNELS;
        int32_t* fft_output_ch2_re = fft_output_ch1_im + NCHANNELS;
        int32_t* fft_output_ch2_im = fft_output_ch2_re + NCHANNELS;

        fft_int_multiple(fft_input_ch1_re, fft_input_ch1_im, fft_output_ch1_re, fft_output_ch1_im);
        fft_int_multiple(fft_input_ch2_re, fft_input_ch2_im, fft_output_ch2_re, fft_output_ch2_im);
    }
}


void compute_fft_for_zoom(int fft_idx, const struct core_state* state,
                          int32_t* ch1_real, int32_t* ch1_imag,
                          int32_t* ch2_real, int32_t* ch2_imag)
{
    int fft_offset = fft_idx * FFT_SIZE;

    int32_t* fft_input_ch1_re = ch1_real + fft_offset;
    int32_t* fft_input_ch1_im = ch1_imag + fft_offset;
    int32_t* fft_input_ch2_re = ch2_real + fft_offset;
    int32_t* fft_input_ch2_im = ch2_imag + fft_offset;


    if (state->cal.use_float_fft) {

        float* fft_output_ch1_re = (float*)(CAL_DATA) + fft_idx * FFT_SIZE;
        float* fft_output_ch1_im = fft_output_ch1_re + NCHANNELS;
        float* fft_output_ch2_re = fft_output_ch1_im + NCHANNELS;
        float* fft_output_ch2_im = fft_output_ch2_re + NCHANNELS;

        fft_float(fft_input_ch1_re, fft_input_ch1_im, fft_output_ch1_re, fft_output_ch1_im);
        fft_float(fft_input_ch2_re, fft_input_ch2_im, fft_output_ch2_re, fft_output_ch2_im);
    } else {

        int32_t* fft_output_ch1_re = (int32_t*)(CAL_DATA) + fft_idx * FFT_SIZE;
        int32_t* fft_output_ch1_im = (int32_t*)(CAL_DATA) + (ZOOM_NFFT + fft_idx) * FFT_SIZE;
        int32_t* fft_output_ch2_re = (int32_t*)(CAL_DATA) + (2 * ZOOM_NFFT + fft_idx) * FFT_SIZE;
        int32_t* fft_output_ch2_im = (int32_t*)(CAL_DATA) + (3 * ZOOM_NFFT + fft_idx) * FFT_SIZE;

        fft_int(fft_input_ch1_re, fft_input_ch1_im, fft_output_ch1_re, fft_output_ch1_im);
        fft_int(fft_input_ch2_re, fft_input_ch2_im, fft_output_ch2_re, fft_output_ch2_im);
    }
}

void correlate_and_accumulate(const struct core_state* state, int fft_idx, void* to_send) {
    if (state->cal.use_float_fft) {
        float* ch1_re = (float*)(CAL_DATA) + fft_idx * FFT_SIZE;
        float* ch1_im = (float*)(CAL_DATA) + (ZOOM_NFFT + fft_idx) * FFT_SIZE;
        float* ch2_re = (float*)(CAL_DATA) + (2 * ZOOM_NFFT + fft_idx) * FFT_SIZE;
        float* ch2_im = (float*)(CAL_DATA) + (3 * ZOOM_NFFT + fft_idx) * FFT_SIZE;

        // write to the same location
        float* ch1_autocorr_float = (float*)(to_send);
        float* ch2_autocorr_float = ch1_autocorr_float + FFT_SIZE;
        float* ch1_2_corr_real_float = ch2_autocorr_float + FFT_SIZE;
        float* ch1_2_corr_imag_float = ch1_2_corr_real_float + FFT_SIZE;

        for(int i = 0; i < FFT_SIZE; i++) {
            if (state->cal.zoom_avg_idx == 0 && fft_idx == 0) {
                ch1_autocorr_float[i] = (sqf(ch1_re[i]) + sqf(ch1_im[i]));
                ch2_autocorr_float[i] = (sqf(ch2_re[i]) + sqf(ch2_im[i]));
                ch1_2_corr_real_float[i] =  (ch1_re[i] * ch2_re[i] + ch1_im[i] * ch2_im[i]);
                ch1_2_corr_imag_float[i] =  (-ch1_re[i] * ch2_im[i] + ch1_im[i] * ch2_re[i]);
            } else {
                ch1_autocorr_float[i] += (sqf(ch1_re[i]) + sqf(ch1_im[i]));
                ch2_autocorr_float[i] += (sqf(ch2_re[i]) + sqf(ch2_im[i]));
                ch1_2_corr_real_float[i] +=  (ch1_re[i] * ch2_re[i] + ch1_im[i] * ch2_im[i]);
                ch1_2_corr_imag_float[i] +=  (-ch1_re[i] * ch2_im[i] + ch1_im[i] * ch2_re[i]);
            }

            if (state->cal.zoom_avg_idx == state->cal.zoom_Navg - 1 && fft_idx == ZOOM_NFFT - 1) {
                ch1_autocorr_float[i] /= (state->cal.zoom_Navg  * ZOOM_NFFT);
                ch2_autocorr_float[i] /= (state->cal.zoom_Navg  * ZOOM_NFFT);
                ch1_2_corr_real_float[i] /= (state->cal.zoom_Navg  * ZOOM_NFFT);
                ch1_2_corr_imag_float[i] /= (state->cal.zoom_Navg  * ZOOM_NFFT);
            }
        }

    } else {
        int32_t* ch1_re = (int32_t*)(CAL_DATA) + fft_idx * FFT_SIZE;
        int32_t* ch1_im = (int32_t*)(CAL_DATA) + (ZOOM_NFFT + fft_idx) * FFT_SIZE;
        int32_t* ch2_re = (int32_t*)(CAL_DATA) + (2 * ZOOM_NFFT + fft_idx) * FFT_SIZE;
        int32_t* ch2_im = (int32_t*)(CAL_DATA) + (3 * ZOOM_NFFT + fft_idx) * FFT_SIZE;

        int32_t* ch1_autocorr_int = (int32_t*)(to_send);
        int32_t* ch2_autocorr_int = ch1_autocorr_int + FFT_SIZE;
        int32_t* ch1_2_corr_real_int = ch2_autocorr_int + FFT_SIZE;
        int32_t* ch1_2_corr_imag_int = ch1_2_corr_real_int + FFT_SIZE;

        for(int i = 0; i < FFT_SIZE; i++) {
            if (state->cal.zoom_avg_idx == 0) {
                ch1_autocorr_int[i] = (sqi(ch1_re[i]) + sqi(ch1_im[i])) / (state->cal.zoom_Navg * ZOOM_NFFT);
                ch2_autocorr_int[i] = (sqi(ch2_re[i]) + sqi(ch2_im[i])) / (state->cal.zoom_Navg * ZOOM_NFFT);
                ch1_2_corr_real_int[i] =  (ch1_re[i] * ch2_re[i] + ch1_im[i] * ch2_im[i]) / (state->cal.zoom_Navg * ZOOM_NFFT);
                ch1_2_corr_imag_int[i] =  (-ch1_re[i] * ch2_im[i] + ch1_im[i] * ch2_re[i]) / (state->cal.zoom_Navg * ZOOM_NFFT);
            } else {
                ch1_autocorr_int[i] += (sqi(ch1_re[i]) + sqi(ch1_im[i])) / (state->cal.zoom_Navg * ZOOM_NFFT);
                ch2_autocorr_int[i] += (sqi(ch2_re[i]) + sqi(ch2_im[i])) / (state->cal.zoom_Navg * ZOOM_NFFT);
                ch1_2_corr_real_int[i] +=  (ch1_re[i] * ch2_re[i] + ch1_im[i] * ch2_im[i]) / (state->cal.zoom_Navg * ZOOM_NFFT);
                ch1_2_corr_imag_int[i] +=  (-ch1_re[i] * ch2_im[i] + ch1_im[i] * ch2_re[i]) / (state->cal.zoom_Navg * ZOOM_NFFT);
            }
        }
    }
}

void dispatch_cal_zoom(struct core_state* state, void* to_send)
{
    struct delayed_cdi_sending* d = &(state->cdi_dispatch);
    wait_for_cdi_ready();

    uint32_t old_appId = d->cal_appId;
    uint32_t old_cal_size = d->cal_size;

    d->cal_appId = AppID_ZoomSpectra;

    // just to be pedantic; it's 4 bytes per entry in both cases
    if (state->cal.use_float_fft)
        d->cal_size = 4 * sizeof(float) * FFT_SIZE;
    else
        d->cal_size = 4 * sizeof(int32_t) * FFT_SIZE;

    memcpy((void*) TLM_BUF, to_send, d->cal_size);

    // TODO: add CRC?
    cdi_dispatch_uC(&(state->cdi_stats),d->cal_appId, d->cal_size);

    //debug_print("z#");

    // restore previous cal_appId, cal_size
    d->cal_appId = old_appId;
    d->cal_size = old_cal_size;
}

void process_cal_zoom(struct core_state* state) {
    void* to_send = (void *)CAL_DATA + 4 * FFT_SIZE * ZOOM_NFFT * sizeof(int32_t);

    static int32_t n_call = 0;
    static const int32_t n_skip = 100000;

    // only start accumulating every 10000 call
    if (state->cal.zoom_avg_idx == 0) {
        n_call++;
        if (n_call <= n_skip) {
            return;
        }
        n_call = 0;
    }

    if (state->cal.zoom_avg_idx < state->cal.zoom_Navg) {
        // after this function finishes, raw PFB outputs will be in CAL_DF
        cal_transfer_data(2);

        int32_t* ch1_real = (int32_t*) CAL_DF;
        int32_t* ch1_imag = ch1_real + NCHANNELS;
        int32_t* ch2_real = ch1_imag + NCHANNELS;
        int32_t* ch2_imag = ch2_real + NCHANNELS;

        n_call++;

#ifdef LN_CORELOOP_FFT_TIMING
        timer_start();
#endif

        for(int fft_batch_idx = 0; fft_batch_idx < ZOOM_NFFT / NUM_FFTS_IN_ONE_GO; ++fft_batch_idx) {
            compute_fft_for_zoom_mult(fft_batch_idx, state, ch1_real, ch1_imag, ch2_real, ch2_imag);
        }

//        for(int fft_idx = 0; fft_idx < ZOOM_NFFT; ++fft_idx) {
//            compute_fft_for_zoom(fft_idx, state, ch1_real, ch1_imag, ch2_real, ch2_imag);
//        }

#ifdef LN_CORELOOP_FFT_TIMING
        uint32_t fft_loop_time = timer_stop();
        debug_print("\r\nFFT_LOOP_TIME = ");
        debug_print_dec(fft_loop_time);
        debug_print("\r\n");
#endif

        // we no longer need data from CAL_DF, all ZOOM_NFFT FFTs are stored at CAL_DATA
        cal_clear_df_flag();

#ifdef LN_CORELOOP_FFT_TIMING
        timer_start();
#endif

        for(int fft_idx = 0; fft_idx < ZOOM_NFFT; ++fft_idx) {
            correlate_and_accumulate(state, fft_idx, to_send);
        }

#ifdef LN_CORELOOP_FFT_TIMING
        uint32_t caa_time = timer_stop();
        debug_print("\r\nCORR_ACC_TIME = ");
        debug_print_dec(caa_time);
        debug_print("\r\n");
#endif

        state->cal.zoom_avg_idx++;
    }

    if (state->cal.zoom_avg_idx == state->cal.zoom_Navg) {
        dispatch_cal_zoom(state, to_send);
        state->cal.zoom_avg_idx = 0;
    }
}
