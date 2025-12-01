## housekeeping.c

Handles telemetry and housekeeping packets:
- `send_hello_packet` - sends startup packet
- `update_heartbeat` - fills heartbeat data
- `process_hearbeat` - sends periodic heartbeat
- `process_housekeeping` - sends various housekeeping data
- `process_eos` - sends end-of-sequence packet

### `send_hello_packet(struct core_state *state)`
Transmits a startup notification packet containing firmware version information, build timestamps, and system identifiers.

### `update_heartbeat(struct core_state *state, struct heartbeat *payload)`
Populates a heartbeat structure with current system telemetry including packet count, timestamps, TVS sensor readings, CDI statistics, error flags, and a magic signature ("BRNMRL").

### `process_hearbeat(struct core_state *state)`
Checks if the heartbeat timer has expired and, if so, constructs and transmits a heartbeat packet via CDI. Reschedules the next heartbeat and increments the packet counter. Returns true when a packet was sent.

### `process_housekeeping(struct core_state *state)`
Handles housekeeping data requests by assembling and transmitting the requested telemetry type. Supports multiple housekeeping formats: full core state dump (type 0), ADC statistics (type 1), system health with heartbeat data (type 2), and calibrator weight checksum (type 3). After transmission, clears the request flag and error masks. Returns true when a packet was successfully sent.

### `process_eos(struct core_state *state)`
Processes End-Of-Sequence requests by first ensuring all pending operations are complete (CDI dispatches finished, no outstanding housekeeping or waveform requests). When ready, constructs and sends an EOS packet containing a unique packet ID and the user-supplied EOS argument value. Clears the request flag after transmission. Returns false if prerequisites aren't met, true when the packet is sent.
