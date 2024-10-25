
# CHANGELOG

# Version 1r06
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