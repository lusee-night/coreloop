## core_loop.c

This is the main firmware file containing the core processing loop. Key functions include:
- `core_loop` - the main outer loop handling initialization and soft resets
- `core_init_state` - initializes the core state structure
- Various helper functions for timing, state management, and system control

### `mini_wait(uint32_t ticks)`
Implements a blocking wait for a specified number of timer ticks.

### `debug_helper(uint8_t arg, struct core_state* state)`
Helper function for debugging that restores a saved state from flash.

### `core_init_state(struct core_state* state)`
Initializes the entire core state structure to default values. This includes setting all hardware interfaces to known states, initializing counters and timers, configuring default gain and averaging parameters, setting up error masks, and preparing the command buffer. The function also loads default Grimm calibration weights and ensures the spectrometer and calibrator are initially disabled.

### `process_waveform(struct core_state* state)`
Handles waveform capture requests by triggering the hardware to capture raw ADC data and sending it via CDI along with timestamp metadata.

### `init_tvs_sensor_accum()`
Initializes the temperature and voltage sensor accumulators to zero.

### `display_greeting(void)`
Prints the firmware version and greeting message to the debug console.

### `process_dispatches(struct core_state* state)`
Coordinates the dispatching of various telemetry packets in priority order: heartbeat, delayed CDI data, housekeeping, waveforms, and end-of-sequence packets. Only processes when CDI is ready.

### `core_loop(struct core_state* state)`
The main firmware loop implementing a two-level structure. The outer loop handles soft resets and system initialization, including hardware setup, state initialization, hello packet transmission, and command buffer clearing. The inner loop performs the core processing: checking for CDI commands, monitoring watchdogs, processing spectrometer data, managing the calibrator, handling automatic gain control, and dispatching telemetry. The loop continues until a reset or shutdown command is received. On shutdown, it ensures all buffers are emptied and any watchdog trip notifications are sent before terminating.

### `MSYS_EI5_IRQHandler(void)`
Timer interrupt handler running at 100Hz. Updates the global tap counter, accumulates TVS sensor readings with averaging, tracks loop performance statistics (min/max loop counts), and clears the timer interrupt flag.

### `update_time(struct core_state* state)`
Updates all time-related fields in the state structure from hardware timers.

### `get_Navg1`, `get_Navg2`, `get_Nfreq`, `get_tr_avg`, `get_gain_auto_max`, `get_tr_length`
Utility functions that compute derived quantities from the state configuration.

### `reset_errormasks(struct core_state* state)`
Clears all error flags and overflow masks.

### `RFS_stop(struct core_state* state)` / `RFS_start(struct core_state* state)`
Stop and start the spectrometer, respectively. Starting clears accumulation buffers and resets statistics.

### `restart_spectrometer(struct core_state* state)`
Convenience function that stops and restarts the spectrometer.

### `trigger_ADC_stat()`
Triggers the hardware to collect ADC statistics samples.

### `timer_start()` / `timer_stop()`
Functions for measuring elapsed time using the hardware timer.
