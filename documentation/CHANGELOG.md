
# CHANGELOG

## Version 1r10
 * Removed global state

## Version 1r09
 * Added ability to take synchronized waveforms at SSL using a large delay in spec_get_waveform

## Version 1r08
* Add 16 bit formats for spectra (10+6 and 4 to 5) to enum output_format
* Add corresponding functions `dispatch_16bit_10_plus_6_data`, `dispatch_16bit_4_to_5_data` to `spectra_out.c`
* Add `value_from_format`, `format_from_value` dicts to pycoreloop
* Set error flag if a command attempts to change spectrometer settings while it is runnning

## Version 1r07
 * Add Github workflow to run uncrater tests on each commit
 * Add encoding/decoding functions to pack 4 int32_t values into 5 uint16_t
 * Fix sending time-resolved data, send in 16 packets that are in correspondence with averaged products
 * Add safe_abs function to handle INT32_MIN correctly

## Version 1r06
 * Revamped how timers are treated
 * Added a real queue for CDI commands. Commands starting with RFS_SPECIAL (0x11) skip the queue (to e.g. reboot)
 * Added more CDI_wait commands (can wait real real long!!)
 * Checking for dropped CDI packets
 * Changed errors and added errors to pycoreloop
 * Command to enable / disable heartbeat
 * Command to control CDI waiting


## Version 1r05
 * Implemented time-resolved spectra 
 * Remove all derived fields to avoid situation when we forget to update them, all derived quantities are now computed by functions.
 * Add encode/decode functions using 10 + 6 scheme (operate on an individual number).
 * Add encode/decode functions that encode the whole array by identifying the segment that can be encoded using the same shift value.
 * Add executable targets to check those in C and a shared library target to test in Python in uncrater.
 *  Minor changes: add two buffers for time-resolved. Replace error-prone ternary operator to get TICK/TOCK buffers by functions spectra_write_buffer/spectra_read_buffer.
 *  Add description of core loop logic to doc/coreloop.md.



## Previous version
 * Updates lost in the mist of prehistory
