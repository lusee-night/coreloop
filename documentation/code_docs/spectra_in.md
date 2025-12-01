1. `get_with_zeros` - helper for getting value and updating leading zero counters
2. `safe_abs_val` - safe absolute value handling INT32_MIN
3. `is_bad_int32` - checks if int32 sample is bad
4. `my_fabsf` - simple float absolute value
5. `is_bad_float` - checks if float sample is bad
6. `is_bad_int40` - checks if 40-bit packed sample is bad
7. `is_bad` - dispatcher for bad value checking
8. `write_spectrum_value` - writes spectrum value to DDR
9. `transfer_from_df` - main transfer function from DF to DDR
10. `transfer_grimm_from_df` - transfers Grimm calibration data
11. `transfer_time_resolved_from_df` - transfers time-resolved spectra
12. `process_spectrometer` - main entry point

# Documentation for spectra_in.c

## `get_with_zeros`
Retrieves the value and updates leading-zero min/max counters used for compression.

## `safe_abs_val`
Returns absolute value, handling INT32_MIN edge case.

## `is_bad_int32`
Evaluates whether a 32-bit integer spectrum sample should be rejected based on comparison with the previous integration's value. First checks if the current value is below a noise threshold (reject_ratio * 2^Navg2_shift). Then compares the current sample against the previous integration's corresponding bin, accounting for whether all previous samples were accepted (affects scaling). The sample is marked as bad if it deviates from the previous value by more than 1/reject_ratio. When CORELOOP_SPECTRA_IN_AVOID_DIV_IN_BAD is defined, uses multiplication instead of division to avoid costly division operations at the expense of potential overflow requiring 64-bit arithmetic.

## `my_fabsf`
Simple absolute value for float, used in bad-value checks.

## `is_bad_float`
Determines if a floating-point spectrum sample should be rejected by comparing it against the accumulated sum from previous integrations. The rejection criterion is whether the absolute difference between the current value and the averaged previous value exceeds 1/reject_ratio of the previous value. When CORELOOP_SPECTRA_IN_AVOID_DIV_IN_BAD is defined, reformulates the comparison to avoid division by multiplying both sides appropriately, trading precision for performance.

## `is_bad_int40`
Checks whether a 40-bit packed integer sample should be rejected. This function handles the extended precision accumulation mode where values are stored as 40-bit signed integers (32 bits magnitude in main buffer, 8 bits including sign in high buffer). Extracts the previous 40-bit value using packed encoding, scales appropriately based on whether all previous samples were accepted, and applies the same rejection criterion as the other bad-value checks. The CORELOOP_SPECTRA_IN_AVOID_DIV_IN_BAD variant performs the comparison in 64-bit arithmetic to avoid division.

## `is_bad`
Dispatcher function that selects the appropriate bad-value checking algorithm based on the configured averaging mode (INT32, INT_40_BITS, or FLOAT). This abstraction allows the main processing loop to remain agnostic to the underlying data representation while still applying consistent rejection criteria across all modes.

## `write_spectrum_value`
Writes a single spectrum sample into the DDR buffer with mode-specific handling. For INT32 mode, immediately divides by 2^Navg2_shift before accumulation to prevent overflow. For FLOAT mode, accumulates the raw value as a float. For INT_40_BITS mode, implements a packed 40-bit accumulator where the low 32 bits are stored in the main buffer and the high 7 bits plus sign bit are packed 4-per-uint32 in a separate high buffer. The 40-bit mode provides extended dynamic range for long integrations while maintaining memory efficiency. The function handles the first sample specially (weight=0) by initializing rather than accumulating.

## `transfer_from_df`
Primary data acquisition function that transfers spectrum data from the FPGA data formatter buffer to DDR memory while applying quality control and accumulation. First performs a rejection check across all enabled auto-correlation products if reject_ratio is configured and sufficient samples have been accumulated (weight > Navg2/2). Counts the number of "bad" bins that deviate significantly from the previous integration and rejects the entire frame if too many bins are bad (exceeds reject_maxbad). For accepted frames, iterates through all 16 correlation products, accumulating enabled products into the active tick/tock buffer with appropriate mode-specific handling. For the first 4 auto-correlation products, also tracks leading zero statistics for potential compression. Updates weight counters and bad-bin statistics. Calls subsidiary functions to handle time-resolved and Grimm calibration data extraction. Returns true if the frame was accepted, false if rejected.

## `transfer_grimm_from_df`
Extracts calibrated power measurements at four rover calibration tone frequencies (14.3625, 20.8875, 29.5625, 41.8125 MHz) for the Grimm's tales diagnostic mode. For each spectrum, computes weighted sums of 8 bins centered around each calibration frequency using configurable weights stored in grimm_weights array. The weights allow for spectral windowing to reduce leakage. Each weighted sum is divided by 1024 for normalization. The four resulting power values per spectrum are then compressed using 4-to-5 encoding (packing four 32-bit values into five 16-bit values) and stored in a dedicated Grimm buffer indexed by the current averaging counter. This provides a time series of calibration tone powers across the integration period.

## `transfer_time_resolved_from_df`
Captures time-resolved spectral snapshots within a configurable frequency range for detailed temporal analysis. Extracts bins from tr_start to tr_stop for each enabled correlation product, with optional frequency averaging controlled by tr_avg and tr_avg_shift parameters. Each extracted bin is encoded as 16-bit using 10+6 mantissa/exponent format to maintain dynamic range while reducing data volume. The time-resolved data is stored in a separate buffer indexed by averaging counter and spectrum number, building up a time series across the Stage-2 integration period. Returns false if the frequency range is invalid (tr_stop <= tr_start) or if the buffer would overflow.

## `process_spectrometer`
Main entry point for spectrometer data processing called each core loop iteration. Checks for new spectrum availability via hardware flag, handles optional frame dropping for synchronization, and orchestrates the complete data flow. When new data arrives, it triggers ADC statistics collection, transfers data from the data formatter to DDR (with rejection filtering), monitors digital overflow flags, and performs automatic bitslice control to optimize dynamic range. Tracks integration progress through avg_counter, and when Stage-2 averaging completes (avg_counter reaches Navg2), it swaps tick/tock buffers, finalizes weight and quality statistics, and initiates CDI packet transmission. Implements safety mechanisms including detection of stuck AGC loops (via bitslicer_action_counter) and overflow conditions. The function maintains critical timing by restarting the spectrometer if bitslice changes occur mid-integration, ensuring data coherency.