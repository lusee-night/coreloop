# Core Loop Overview

The **coreloop** directory implements the firmware‑level processing for the LuSEE‑Night radiospectrometer.  Its primary responsibilities are:

* **Acquire raw data** from the FPGA data‑formatter (DF) buffers.
* **Perform averaging, filtering, and compression** on the spectra.
* **Manage calibrator and gain control** hardware, including automatic bit‑slicing and SNR lock‑in.
* **Prepare and transmit CDI packets** (metadata, spectra, time‑resolved data, Grimm calibration, and calibrator packets) to the spacecraft telemetry system.

The code is written in C and is highly performance‑oriented – many operations are done in‑place on DMA buffers, and the processing loop is driven by a state machine (`core_state`).  The surrounding `src/` directory contains a simulated environment for running the firmware on a normal computer, but the real target is the embedded hardware.

---

## High‑Level Architecture

```
+-------------------+       +-------------------+       +-------------------+
|  Spectra In (src) | ----> |  Core Loop (c)    | ----> |  Spectra Out (c) |
+-------------------+       +-------------------+       +-------------------+
```

* **spectra_in.c** – Reads the DF buffer, applies rejection criteria, accumulates spectra, and updates leading‑zero statistics.  It also handles time‑resolved spectra and the Grimm calibration path.
* **core_loop.c** – The main loop (`core_loop`) that drives the state machine, checks for new data, triggers ADC statistics, and coordinates the various subsystems.
* **gain.c** – Implements automatic analog gain control and bit‑slice control based on the current spectrum statistics.
* **housekeeping.c** – Sends periodic telemetry such as heartbeat packets and watchdog status.
* **watchdogs.c** – Monitors temperature and watchdog flags, reporting any trips.
* **calibrator.c** – Manages the calibrator hardware, including mode handling, SNR thresholds, slice adjustments, and packetization of calibration data.
* **calibrator_zoom.c** – Specialized zoom‑mode processing: performs FFTs on a subset of channels, correlates results, and dispatches zoom spectra.
* **commanding.c** – Parses incoming CDI commands and performs actions such as soft‑reset or configuration changes.
* **spectra_out.c** – Packages processed spectra into CDI packets, handling different output formats (32‑bit, 16‑bit packed, shared‑LZ, etc.) and sends them.
* **fft.c / utils.c** – Low‑level FFT implementations and helper functions for encoding/decoding data.

---

## Core Data Structures

* `struct core_state` – Central state containing configuration (`base`), timing, packet counters, and sub‑structures for calibrator, gain, watchdog, etc.
* `struct calibrator_state` – Holds calibrator mode, averaging parameters, SNR thresholds, slice positions, and zoom configuration.
* `struct cdi_stats` – Tracks how many CDI packets have been sent and bytes transmitted.
* `struct meta_data` – Metadata packet that includes firmware version, unique packet ID, and a snapshot of the current `core_state.base`.

These structures live in **core_loop.h** and are accessed throughout the code base; most functions take a `struct core_state*` argument so they can read or update the global state.

---

## Processing Flow (Simplified)

1. **Check for new spectrum** – `process_spectrometer` in `core_loop.c` calls `spec_new_spectrum_ready()`.
2. **Handle drops** – If a frame should be dropped, the DF flag is cleared.
3. **Transfer data** – `transfer_from_df` (in `spectra_in.c`) copies raw samples into DDR buffers, applying bad‑value rejection based on `state->base.reject_ratio` and averaging settings.
4. **Gain & Bit‑Slice** – `bitslice_control` (in `gain.c`) may adjust slices; `analog_gain_control` may adjust analog gain.
5. **Averaging complete?** – When `avg_counter` reaches `get_Navg2(state)`, the code toggles `tick_tock`, updates weight counters, and calls `transfer_to_cdi`.
6. **Prepare CDI** – `transfer_to_cdi` builds a metadata packet, selects the next application IDs, and sets up the dispatch descriptor.
7. **Dispatch** – `process_delayed_cdi_dispatch` iterates over products, time‑resolved packets, Grimm data, and calibrator data, sending each via `dispatch_data`, `dispatch_tr_data`, etc.
8. **Housekeeping** – Periodic watchdog and heartbeat packets are sent in parallel to keep the ground station informed.

---

## Extending the Code

* **Adding a new CDI command** – Implement the handling in `commanding.c` and update the command table in `lusee_commands.h`.
* **New output format** – Extend `prepare_spectrum_packet` and `dispatch_data` in `spectra_out.c` to support the format, and add an encoder in `utils.c` if needed.
* **Simulation** – The `src/` directory provides a mock FPGA interface; you can run the main loop on a workstation for testing.

---

## Where to Start

* **Read `core_loop.c`** – It shows the main loop and how the subsystems are orchestrated.
* **Look at `spectra_in.c` and `spectra_out.c`** – These files contain the data path from raw acquisition to CDI transmission.
* **Check `documentation/coreloop.md`** – It contains a higher‑level description of the core loop architecture.
* **Run the simulation** – Build the `src/` program (`make` in the repository root) to see the flow in action.

---

## Further Reading

* `documentation/lusee_appIds.md` – List of CDI application IDs used throughout the code.
* `documentation/toplevel_commands.md` – Overview of the command protocol.
* `core_loop.h` – Declarations of all state structures and helper macros.

---

*This document aims to give new developers a quick mental model of the core loop. Dive into the individual source files for implementation details and the inline comments added throughout the code.*