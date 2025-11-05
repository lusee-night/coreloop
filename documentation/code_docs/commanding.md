## commanding.c

This file implements the CDI (Command and Data Interface) command processing system for the LuSEE-Night firmware. It receives and processes commands, updating the core state and controlling various subsystems.  It contains:
- `cdi_not_implemented` - debug function
- `cmd_soft_reset` - performs soft reset
- `process_cdi` - main command processing function with a large switch statement

The main function `process_cdi` reads CDI commands and processes them according to their type. It handles various system configurations like gain settings, averaging modes, calibrator settings, etc.

### `cdi_not_implemented(const char *msg)`
Outputs a debug message for unimplemented CDI commands.

### `cmd_soft_reset(uint8_t arg_low, struct core_state* state)`
Executes a soft reset of the system by stopping the spectrometer, asserting the hardware reset, and setting the soft reset flag. The `arg_low` parameter controls whether stored states should be preserved after reset.

### `process_cdi(struct core_state* state)`
The main command processing function that handles incoming CDI commands. It first checks for new commands in the buffer and handles special immediate commands (like reset and sequence upload). Regular settings commands are queued in a circular buffer for later execution. The function then processes queued commands when the system is ready (not waiting, not uploading sequences, and buffers are clear). The giant switch statement handles dozens of command types including: spectrometer start/stop, gain and routing configuration, averaging settings, output format selection, calibrator control, watchdog management, and debug functions. Commands that change critical settings while the spectrometer is running are rejected with error flags. The function returns true only when a "time-to-die" command is received, signaling the main loop to terminate.

