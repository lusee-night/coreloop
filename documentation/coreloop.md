## General description of core loop logic

Each iteration of the core loop does the following:
1. `process_cdi`. This function receives commands and applies them (changing
   spectrometer settings, etc).
2. `process_spectrometer`. Add raw data from FPGA to one of the `TICK`/`TOCK`
   buffers.
3. `process_gain_range`. Deals with gain stuff.
4. Do only one of the following (in that order):
    1. `process_heartbeat`. Send heartbeat package.
    2. `delayed_cdi_dispatch`. Send averaged data. More precisely, copy it
    from `TOCK`/`TICK` buffer to CDI buffer and call `cdi_dispatch`.
    3. `process_housekeeping`.
    4. `process_wave_form`.

## Interrupts

Interrupts happen every 100 ms.
Every interrupt will decrement (if positive):
 - `resettle_counter`
 - `state.cdi_dispatch.int_counter`
 - `heartbeat_counter`


## `process_spectrometer`

- Determine if we use current packet (should not differ too drastically from the previous ones).
- If we use it: add current packet (`df_ptr`) divided by `2 ** Navgf` to the
current `TICK-TOCK` buffer.

## Packet metadata

### AppId

There are 3 categories: High, Medium and Low. AppId is updated
cyclically: take the base value of one of the categories. Say, we
are sending the `i`-th product, `0 <= i <= 15`. AppId is `base value + i`.

`cdi_dispatch` takes AppId as a parameter. We store it in
`state.cdi_dispatch.app_id`. It is set in `transfer_to_cdi` function
from `get_next_baseAppId` and then incremented in
`process_delayed_cdi_dispatch`, if this function actually called
`cdi_dispatch`.


### `packet_id`

This *uniquely identifies* the main (averaged data) packet. NB: time-resolved
spectra must have the same `packet_id` as the corresponding averaged data packet.

There is a global variable `unique_packet_id`. It is incremented
in each call to `transfer_to_cdi`. After that it is assigned to
`state.cdi_dispatch.packet_id` and is saved in the first 4 bytes of the
outgoing packet. In contrast to AppId, `cdi_dispatch` knows nothing about `packet_id`: to `cdi_dispatch` it
is just another part of the binary blob.

## TICK/TOCK

In `process_spectrometer`, we keep accumulating new incoming data
in one of these buffers. When dispatching, we copy data
from the other one. Once we dispatched all data, the roles of the buffers
are swapped. This is regulated by global `tick_tock` bool flag.
For convenience, there are functions `spectra_write_buffer`
and `spectra_read_buffer`. They return pointers to the buffer
to which we must write and read in the current iteration. The
argument must be the global `tick_tock` flag. 

TODO: change to no argument?

For time-resolved data it's the same: `tr_spectra_write_buffer` and `tr_spectra_read_buffer`
functions will return the pointers to the correct buffer.
