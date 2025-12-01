1. `cdi_dispatch_uC` - small function that updates stats and calls cdi_dispatch
2. `send_metadata_packet` - sends metadata packet
3. `get_shift` - computes shift amount based on settings
4. `prepare_spectrum_packet` - prepares CDI spectrum packet
5. `dispatch_data` - dispatches spectrum products using different formats
6. `dispatch_grimm_data` - dispatches Grimm calibrated spectra
7. `dispatch_tr_data` - dispatches time-resolved spectra
8. `get_next_baseAppID` - generates next application ID
9. `get_next_tr_baseAppID` - generates next time-resolved application ID
10. `transfer_to_cdi` - main function that prepares all data for CDI transmission
11. `delayed_cdi_dispatch_done` - checks if all packets have been sent
12. `process_delayed_cdi_dispatch` - main dispatch loop that sends all pending packets

This file is responsible for the output side of the spectrometer data flow - packaging and sending processed spectra as CDI packets to the spacecraft telemetry system.

# Documentation for spectra_out.c

## `cdi_dispatch_uC`
Sends a CDI packet for a given app ID and length, updating stats counters.

## `send_metadata_packet`
Constructs and sends a metadata packet containing the firmware version ID, unique packet ID, and a complete snapshot of the current core state base configuration. This packet provides context for interpreting the subsequent spectrum data packets. After sending, it resets any accumulated error masks in the state to start fresh for the next integration period.

## `get_shift`
Computes shift amount based on Navgf and Navg2_shift settings for averaging; used in spectrum extraction.

## `prepare_spectrum_packet`
Sets up the CDI packet structure for spectrum data transmission. This function calculates the data size based on the output format (32-bit, various 16-bit encodings, or 4-to-5 compression), writes the packet ID header, reserves space for CRC, and returns pointers to where the actual spectrum data and CRC value should be written. The packet layout is: [packet_id][CRC][spectrum_data].

## `dispatch_data`
Dispatches a single spectrum product (one of the 16 correlation products) as a CDI packet. This function reads data from the appropriate tick/tock DDR buffer, applies frequency averaging if configured, and encodes the spectrum according to the selected output format. Supported formats include 32-bit raw values, 16-bit with 10+6 mantissa/exponent encoding, and 4-to-5 compression where four 32-bit values are packed into five 16-bit values. For averaging modes that use extended precision (40-bit or float), it calls `get_averaged_value` to properly extract and shift the accumulated values. After encoding, it computes a CRC over the data portion and dispatches the complete packet via CDI.

## `dispatch_grimm_data`
Sends the Grimm's tales calibration spectra accumulated during the current integration period. The Grimm mode extracts power at specific calibration tone frequencies (14.3625, 20.8875, 29.5625, 41.8125 MHz) across all averaging intervals. The data is read from a dedicated Grimm buffer, packed using 4-to-5 encoding, and sent with a packet ID header. The packet size depends on the number of stage-2 averaging periods (Navg2).

## `dispatch_tr_data`
Transmits time-resolved spectra for a single correlation product. Time-resolved mode captures spectrum snapshots at each stage-2 averaging interval within the configured frequency range (tr_start to tr_stop). This function iterates through all Navg2 averaging frames for the current spectrum index, copying each frame's data into a contiguous CDI packet. After sending all 16 spectrum products' time-resolved data, it zeroes the source buffer to prepare for the next integration. Each packet includes a packet ID, CRC, and the concatenated time-resolved snapshots.

## `get_next_baseAppID`
Generates the next base application ID for spectrum packets using a linear congruential generator (LCG) for pseudo-random selection. Based on configured fractions (hi_frac, med_frac), it probabilistically assigns packets to high, medium, or low priority telemetry streams. This allows dynamic bandwidth allocation where critical data can be prioritized when downlink capacity is limited.

## `get_next_tr_baseAppID`
Generates the next time-resolved application ID based on random state and configured fractions.

## `transfer_to_cdi`
Main coordination function that initiates the CDI packet transmission sequence after spectrum averaging is complete. It increments the unique packet ID, captures current timestamps, sends the metadata packet, and then configures the delayed dispatch state for sending all data products. This includes resetting product counters, determining which data types to send (normal spectra, time-resolved, Grimm calibration), calculating frequency dimensions with averaging, selecting application IDs based on priority fractions, and setting the dispatch timer. The actual packet transmission happens later through `process_delayed_cdi_dispatch` to avoid blocking the real-time processing loop.

## `delayed_cdi_dispatch_done`
Checks whether all delayed CDI dispatch components have been sent. Returns true only when the required number of spectrum products, time-resolved spectra, Grimm spectra, and calibrator packets have been dispatched. These thresholds are defined by the constants NSPECTRA and NCALPACKETS and the state counters.

## `process_delayed_cdi_dispatch`
Implements the state machine for delayed packet transmission, called periodically from the core loop when the dispatch timer expires. This function sends one packet per invocation to avoid blocking, cycling through: (1) the 16 correlation products filtered by corr_products_mask, (2) time-resolved spectra if enabled, (3) Grimm calibration data if enabled, and (4) calibrator packets if the calibrator is active. It updates the appropriate counter after each transmission and resets the dispatch timer for the next interval. The function returns true while packets remain to be sent, allowing the core loop to continue spectrum acquisition in parallel with telemetry transmission. This non-blocking approach ensures that the real-time data acquisition is not disrupted by the potentially slow CDI interface.