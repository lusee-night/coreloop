1. `calibrator_default_state` - Initializes calibrator state with default parameters
2. `set_calibrator` - Configures hardware calibrator registers based on state
3. `calibrator_set_SNR` - Sets SNR lock thresholds
4. `calibrator_set_slices` - Sets bit slicer positions
5. `get_mode11_minmax_signed` - Gets min/max for signed values in mode 11
6. `get_mode11_minmax_unsigned` - Gets min/max for unsigned values in mode 11
7. `get_mode11_positive_count` - Counts positive values in mode 11
8. `get_mode11_lock_count` - Counts locked samples
9. `copy_cal_metadata` - Copies calibrator metadata for packets
10. `packetize_mode11_raw` - Packages raw mode 11 data
11. `packetize_mode11_processed` - Packages processed mode 11 data
12. `process_cal_mode11` - Processes mode 11 calibrator data
13. `process_cal_mode00` - Processes mode 0 calibrator data
14. `process_cal_mode_01_10` - Processes mode 1 or 10 calibrator data
15. `check_range_unsigned` - Checks range for unsigned values
16. `check_range_signed` - Checks range for signed values
17. `return_to_bitslicer_settle` - Returns to bit slicer settling mode
18. `process_calibrator` - Main calibrator processing function
19. `dispatch_calibrator_data` - Dispatches calibrator data packets

# Documentation for calibrator.c

## `calibrator_default_state`
Initializes calibrator state with default parameters including averaging factors (Navg2=1, Navg3=10), drift guard bands, antenna mask, SNR thresholds, and bit slicer positions. Sets up initial values for zoom mode configuration, raw data output frequency (raw11_every=20), and enables automatic slice adjustment. These defaults establish a conservative starting point that the auto-calibration algorithms can optimize from.

## `set_calibrator`
Configures the hardware calibrator interface with parameters from the calibrator state structure. This includes setting averaging windows, drift guard/step values, antenna mask, notch filter index, SNR lock thresholds, settling time, delta drift correlations, PFB index, and maximum drift/phase guards. For special modes like bit slicer settling, it overrides some parameters (e.g., sets Navg2=0 for faster convergence). Clears the calibrator data buffer and DF flag to prepare for new acquisitions.

## `calibrator_set_SNR`
Sets calibrator SNR lock thresholds based on calibrator state fields SNRon and SNRoff.

## `calibrator_set_slices`
Configures the bit slicer positions for various calibrator data paths including power top/bottom, sum channels, frequency/second difference, and product channels. The delta_powerbot_slice parameter allows offsetting the power bottom slice relative to power top to maintain consistent dynamic range across both measurements.

## `get_mode11_minmax_signed`
Extracts minimum and maximum values for signed 32-bit data from mode 11 calibrator registers. Processes 1024 samples per antenna (4 antennas total) at the specified register offset within the calibrator data buffer. Used to determine the dynamic range of frequency difference (FD) and second difference (SD) measurements for automatic bit slicer adjustment.

## `get_mode11_minmax_unsigned`
Extracts minimum and maximum values for unsigned 32-bit data from mode 11 calibrator registers. Similar to the signed version but handles unsigned quantities like SNR and power measurements. The implementation includes a workaround for a compiler optimization issue where direct array assignment could be incorrectly optimized, requiring an intermediate copy through local arrays.

## `get_mode11_positive_count`
Counts the number of positive values in a mode 11 register across all 4 antennas, with each antenna having 1024 samples. This is primarily used to monitor the second difference (SD) channel where the ratio of positive to negative values indicates lock quality - in steady state lock, this should be approximately 50/50 following a binomial distribution.

## `get_mode11_lock_count`
Counts the number of active lock flags in the first 1024 samples of the calibrator data buffer, summing across all enabled antennas. A low lock count indicates the calibrator has lost synchronization with the calibration signal and needs to reacquire.

## `copy_cal_metadata`
Assembles a complete calibrator metadata packet including version ID, timestamps, current mode, SNR thresholds, all bit slicer positions, error flags, and statistics. Compresses the drift data by subsampling (every 8th sample) and applying optimal bit shifting to fit into 16-bit values while preserving maximum precision. Calculates per-antenna lock counts by iterating through the lock flags. Copies hardware error registers for diagnostic purposes. This metadata provides ground systems with complete context for interpreting the calibrator data products.

## `packetize_mode11_raw`
Prepares raw mode 11 calibrator data for CDI transmission, used when full diagnostic data is requested (controlled by raw11_every counter). Repacks the 32-bit lock flags into 16-bit format to save space, then inserts the standard calibrator metadata at offset 2KB. Copies the remaining raw register data (starting from register 1) unchanged. This raw format preserves all calibrator measurements for detailed ground analysis but requires significant telemetry bandwidth (24KB total).

## `packetize_mode11_processed`
Creates a compact mode 11 packet containing only the calibrator metadata without raw data. This is the standard operating mode that provides essential calibrator health and performance metrics while minimizing telemetry usage. The metadata includes compressed drift measurements, lock statistics, bit slicer positions, and error flags - sufficient for routine monitoring and trending.

## `process_cal_mode11`
Handles the complete mode 11 processing pipeline: transfers data from hardware, captures error flags, computes statistics (SNR/power ranges, FD/SD ranges, SD positive counts, lock counts), and determines whether to send raw or processed packets based on the raw11_counter. The statistics are used by the auto-calibration algorithms to adjust bit slicers and SNR thresholds. Increments the raw data counter and resets it when raw11_every is reached, implementing a decimated raw data cadence.

## `process_cal_mode00`
Processes mode 0 calibrator data which contains phase and amplitude measurements for carrier tracking. Validates that at least half the integration period contains valid data (by checking gNacc counter) before accepting the packet. Copies the complete 20KB dataset (5 chunks of 4KB plus 1 register) to the calibrator data buffer and configures dispatch parameters for segmented transmission. Mode 0 provides the primary science data for phase calibration.

## `process_cal_mode_01_10`
Handles raw PFB (Polyphase Filter Bank) data for modes 1 and 10, which contain complex FFT outputs for detailed spectral analysis. Transfers 64KB of data (4 channels × 2048 samples × 2 (real+imaginary) × 4 bytes) directly to the calibrator buffer without processing. These modes are primarily used for commissioning and diagnostic purposes to verify PFB performance.

## `check_range_unsigned`
Determines the minimum bit shift required to scale unsigned measurement values to fit within a target range. Iterates through all enabled antennas to find the maximum shift needed across all channels, ensuring no data overflow when bit slicers are adjusted. Returns 0 if values are already in range, otherwise returns the number of bits to shift right. This prevents digital saturation in the calibrator signal processing chain.

## `check_range_signed`
Similar to check_range_unsigned but handles signed values by considering both maximum and minimum values (absolute value of the most extreme). Used for bipolar measurements like frequency difference (FD) and second difference (SD) where both positive and negative excursions must fit within the bit slicer's dynamic range.

## `return_to_bitslicer_settle`
Initiates a return to bit slicer settling mode when measurements indicate the current slicer positions are causing overflow or underflow. Applies conservative adjustments (adding 7 to powertop_slice, 5 to sum slices and fd_slice) to back off from the saturation point. Resets the calibrator hardware, configures fast settling mode (Navg2=0), and disables SNR lock to allow rapid convergence to proper levels.

## `process_calibrator`
Main calibrator state machine that orchestrates all calibration modes and transitions. In ZOOM mode, delegates to specialized zoom processing. In BIT_SLICER_SETTLE mode, iteratively adjusts slicers based on signal statistics until convergence or timeout, prioritizing power top, then sum channels, then frequency difference. In SNR_SETTLE mode, analyzes SNR statistics to determine optimal lock thresholds, setting SNRon to 90% of the observed range when signal is present. In RUN mode, continuously monitors for bit slicer errors and lock quality, automatically returning to settling modes if problems are detected. The function implements sophisticated auto-calibration logic including detection of lock loss (using SD positive count statistics), automatic bit slicer adjustment for product channels, and graceful degradation when calibration signals are absent. Raw modes (0-3) bypass auto-calibration for manual operation or diagnostics.

## `dispatch_calibrator_data`
Sends calibrator packets to CDI with mode-specific handling. For metadata packets (AppID_Calibrator_MetaData), sends the complete pre-formatted metadata structure in a single transmission. For data packets, adds a 12-byte header (packet ID and timestamps) and sends in segments determined by cal_packet_size. For debug mode packets, applies RLE (run-length encoding) compression to reduce data volume, particularly effective for the sparse lock flag data. Handles packet segmentation for large datasets, incrementing the app ID for each segment to enable ground system reassembly. Updates the cal_count to track transmission progress, setting it to 0xFE when the last packet is sent to signal completion.