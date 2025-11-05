## gain.c

This file handles automatic gain control:
- `update_spec_gains` - updates hardware with gain settings
- `set_route` - configures channel routing
- `analog_gain_control` - automatic analog gain control based on ADC statistics
- `process_gain_range` - processes ADC range and handles gain updates
- `bitslice_control` - digital gain control via bit slicing


### `update_spec_gains(struct core_state* state)`
Programs the hardware gain registers for all input channels using the values stored in `state->base.actual_gain`.

### `set_route(struct core_state* state, uint8_t ch, uint8_t arg_low)`
Configures the analog routing for a specific channel, setting which physical inputs are connected to the plus and minus sides of the differential input.

### `analog_gain_control(struct core_state* state)`
Implements automatic analog gain control by examining ADC statistics for each channel. When AGC is enabled for a channel, it compares the peak signal amplitude against configured thresholds. If the signal exceeds the maximum threshold, gain is decreased; if below the minimum threshold, gain is increased. The function tracks whether any gain changes occurred and updates error flags to indicate AGC actions or limit conditions. Returns true if any gain was changed, triggering a hardware update.

### `process_gain_range(struct core_state* state)`
Manages the overall gain adjustment process. When new ADC statistics are available, it calls the AGC function. If gains change, it schedules a resettle period before restarting the spectrometer to allow the analog circuitry to stabilize. Also handles manual range check requests for housekeeping telemetry.

### `bitslice_control(struct core_state* state)`
Performs automatic digital gain control through bit-slicing. For channels in auto mode, it examines leading-zero statistics to determine how many significant bits the data contains. It then adjusts the bit slice position to keep the configured number of bits, maintaining a 2-bit hysteresis buffer to prevent oscillation. Cross-correlation products have their bit slices automatically computed as averages of their constituent channels. Returns true if any bit slice changed.
