## sequencer.c

Manages spectrometer configuration through core initialization functions.

### `set_spectrometer(struct core_state* state)`
Configures the spectrometer hardware according to the current core state settings. Programs the analog gain and input routing for each of the NINPUT channels, sets the bit-slicing configuration for data compression, configures the first-stage averaging shift factor, enables and configures the notch filter with optional subtraction disable and detector enable flags, and finally initializes the calibrator subsystem. This function translates the software state into actual hardware register writes and is called whenever the spectrometer configuration needs to be updated.

### `default_state(struct core_state_base *base)`
Initializes a core_state_base structure with safe default values for system startup. Sets all input channels to medium gain with direct routing (channel N routed to input N with ground reference), configures 14-bit shift for stage 1 averaging and 3-bit shift for stage 2, disables frequency averaging (Navgf=1), sets all bit slicers to keep the 5 MSBs (0x1F), disables the notch filter, sets fraction thresholds to maximum ranges, configures 32-bit output format, disables bad value rejection, and disables time-resolved mode. These defaults provide a conservative starting configuration that can be modified via commands.
