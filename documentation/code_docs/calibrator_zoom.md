# Documentation for calibrator_zoom.c

1. `sqf` - Simple square function for floats
2. `sqi` - Simple square function for int32
3. `compute_fft_for_zoom_mult` - Computes FFTs for a batch of FFTs in zoom mode
4. `compute_fft_for_zoom` - Computes FFT for a single zoom FFT index
5. `correlate_and_accumulate` - Correlates FFT results and accumulates them
6. `dispatch_cal_zoom` - Dispatches zoom calibration data via CDI
7. `pfb_channel_data_real` - Returns pointer to PFB channel data
8. `process_cal_zoom` - Main processing function for zoom calibration


## `sqf`
Simple square function for floats, used in bad-value checks.

## `sqi`
Simple square function for int32, used in bad-value checks.

## `compute_fft_for_zoom_mult`
Performs a batch of FFTs for zoom mode processing on selected PFB (polyphase filter bank) channel pairs. This function processes NUM_FFTS_IN_ONE_GO FFTs simultaneously for improved performance through better cache utilization and potential vectorization. Takes raw time-domain data from the specified batch index offset in the input buffers (ch1_real/imag, ch2_real/imag) and computes their frequency-domain representations. The function supports both floating-point and integer FFT implementations based on the USE_FLOAT_FFT flag. For float mode, the FFT outputs are written to CAL_DATA buffer with channel 1 real/imaginary parts followed by channel 2 real/imaginary parts, all stored contiguously. For integer mode, the layout differs with all real parts stored first, then all imaginary parts. The batch processing approach significantly reduces FFT computation overhead compared to single FFT calls, which is critical for real-time performance when processing multiple zoom windows.

## `compute_fft_for_zoom`
Computes a single FFT for zoom mode calibration on two PFB channels simultaneously. This function serves as a fallback or debugging alternative to the batch FFT processing. It takes time-domain samples from the specified FFT index offset and transforms them to frequency domain for both calibrator channels. Like the batch version, it supports both float and integer FFT modes with different memory layouts in the output buffer. For float FFTs, the output is interleaved (ch1_re, ch1_im, ch2_re, ch2_im) while integer FFTs use a separated layout where all real components are stored before imaginary components. This separation in integer mode helps with subsequent correlation operations and memory access patterns. The function is typically not used in production but retained for testing individual FFT computations.

## `correlate_and_accumulate`
Performs correlation and power spectrum computation on FFT outputs from two calibrator channels, implementing the core zoom mode spectral analysis. This function computes three products for each frequency bin: the auto-correlation power of channel 1, auto-correlation power of channel 2, and the complex cross-correlation between channels. These products enable both individual channel spectral analysis and phase/coherence measurements between channels. The function implements averaging across multiple FFT frames (controlled by zoom_avg_idx and zoom_Navg) to reduce variance and improve SNR. For the first FFT in an averaging sequence (zoom_avg_idx==0, fft_idx==0), it initializes the accumulation buffers; otherwise it adds to existing values. On the final FFT of the final averaging frame, it normalizes the accumulated values by dividing by the total number of FFTs (zoom_Navg * ZOOM_NFFT). The float path maintains full precision throughout accumulation, while the integer path performs division at each step to prevent overflow. The correlation products are stored in the to_send buffer in the order: ch1_autocorr, ch2_autocorr, ch1_2_corr_real, ch1_2_corr_imag, each containing FFT_SIZE frequency bins.

## `dispatch_cal_zoom`
Transmits accumulated zoom mode calibration spectra to the CDI telemetry system. This function packages the four correlation products (two auto-correlations and one complex cross-correlation) computed by correlate_and_accumulate into a CDI packet with appropriate metadata. The packet header includes a unique packet ID for tracking and the current PFB channel index plus zoom offset to identify which frequency band is being analyzed. The function temporarily overrides the normal calibrator CDI settings (AppID and size) to use zoom-specific values (AppID_ZoomSpectra), then restores them after transmission to avoid disrupting regular calibrator packet flow. The data payload is 4 * FFT_SIZE * sizeof(float or int32) bytes, containing all frequency bins for all correlation products. This packet provides high-resolution spectral information for a narrow frequency band, enabling detailed RFI characterization and calibration source analysis.

## `pfb_channel_data_real`
Returns pointer to the real part of PFB channel data for the specified channel.

## `process_cal_zoom`
Main orchestrator for zoom mode calibration processing, implementing a complete cycle of data acquisition, FFT computation, correlation, averaging, and telemetry dispatch. This function manages a multi-stage pipeline: First, it transfers raw PFB data from the calibrator data formatter into working memory. If differential mode is enabled (zoom_diff_1/2 flags), it subtracts reference channels to remove common-mode signals or create null measurements. Next, it performs FFTs on ZOOM_NFFT consecutive time segments using batch processing for efficiency. The FFT outputs are then correlated and accumulated across zoom_Navg averaging cycles to build up SNR. Once averaging is complete, the correlation products are dispatched via CDI. The function also implements frequency scanning capability through zoom_ndx_range: after completing one frequency band, it advances to the next by updating the PFB index, allowing systematic coverage of a wider frequency range at high resolution. The zoom_avg_idx state variable tracks progress through the averaging sequence, with -1 indicating a PFB settling period after frequency changes. For CPU simulation builds (NOTREAL defined), artificial delays are inserted to mimic hardware timing constraints. The function integrates tightly with the calibrator state machine, being called when mode==CAL_MODE_ZOOM, and coordinates with the PFB hardware through cal_transfer_data and calib_set_PFB_index calls.
