# Test status

![Automated tests now](https://github.com/lusee-night/coreloop/actions/workflows/uncrater_tests.yml/badge.svg)

# coreloop
Core loop of the LuSEE-Night uC flight software.

# To compile

To compile, make sure you have cmake installed. To make the build environment
please issues

```
cmake -B build
```

To build or rebuild the code, use

```
cmake --build build
```

Alternatively, VS code should pick up the configuration files in .vscode. You can compile/run withing the VS code environemnt.
Pressing Ctrl+Shift+P and then issuing `Cmake:Build` command will rebuild the codebase.


# Code organization

The directory `coreloop` contains the actual coreloop code that is shared between this repo and the actual flight software repo. Directories `src` and `include` contain the support code that is neccessary to run `coreloop` in a test environment.

The code in coreloop directory is soft-linked into the [lusee_MiV](https://github.com/lusee-night/lusee_MiV), which is the actual flight software, into the directory `LuSEE_FS/application`.  

## Spectrometer functionality abstraction

The spectrometer functionality, such as enabling the spectrometer mode, asking for a raw ADC packet, etc is abstracted in a set of routines defined in `spectrometer_interface.h`. The functions are implemented in the corresponding source files in the `src` directory in the `coreloop` packages and provide a suitable test harness. At the same time, they are implemented in `lusee_MiV` packates in the `LuSEE_FS/application` directory where they provide the interface to the actual hardware.

## Spectrometer commanding, data and pycoreloop.

Spectrometer commands have 1 byte command and 2 byte argument. The high level command fall all under the `RFS_SETTINGS` (0x10) command. This is majorly so that our high level commanding ICD is fixed.
In this case we use the upper 8 bits of the two bytes as the sub-commands, or more colloquialy in the coreloop context as spectrometer command and one byte as an argument. This is a very compressed representation. The *reference place for command definitions* is under the coreloop repo in [`documentation/lusee_commands.md`](documentation/lusee_commands.md). Definitions changes should always happen in that table. Then cmake is executed in the coreloop directory, and old school make is executed in `pycoreloop` in that directory which generates the appropriate `lusee_commands.py` and `lusee_commands.h`. `lusee_commands.py` is imported in the `pycoreloop` library. This library is imported into the high-level scription model implemented in the uncrater packages [`lusee_script.py`](https://github.com/lusee-night/uncrater/blob/main/scripter/lusee_script.py). `lusee_script.py` abstracts away the cumbersome two-byte commanding into something that is human readable. By for exampling calling `set_anagain('MMMM')` the command to set the analog gain to medium in all 4 analog channels. This refers to `lusee_commands.RFS_SET_GAIN_ANA_SET` which in turns ultimately picks up the actual subcommand number from the markdown table in the documentation. 

The data generated by the spectrometer are return in packets which (under the all the CCSDS cruft) are defined by a binary blob and an appID. AppId is an identifier that tells you what kind of information is in the binary blob. This allows you to interpret that packet correclty. It very important to realize, that the architecture is not command - response one. We cannot afford to do any interfactive commanding on the moon and the DCB will at program the spectrometer and then keep it hads away.  Yes, there are packets to e.g. request a housekeeping of a certain type but all this implies that there will be a housekeeping packet sent sometime soon. For example, if the spectrometer is in the middle of sending spectrometer data, there could be two more data packets and a hearbeat packet comming between the housekeeping command being issued and housekeeping packet being received. The spectrometer is assigned AppIds 0x2XX. Similarly to commands, the AppIDs are defined in mark down table in the coreloop packages [`documentation/lusee_appIDs.md`](documentation/lusee_appIds.md) and similarly `lusee_appIds.h` and `lusee_appIds.py` are auto-generated from that reference markdown package. The actual contents for a given appID is defined by `coreloop.h` mostly as packed C-structures, although for some data one actually needs to dig into the code to understand how it fits together. On the python side, these packets are interpreted using the `uncrater` module. `uncrater` module uses pycoreloop for two things. First, it uses `pycoreloop.appId` and associated dictionaries to link appIDd descritpions with their numerical values. Second, it uses `pycore.pystruct` to import the C-structures that are in these packages. `pycore.pystrcuct` is again programmatically generated from `coreloop/core_loop.h` using [ctypesgen](https://github.com/ctypesgen/ctypesgen) to generate python wrapper for coreloops's C structures. For example, look at [`PacketHello.py`](https://github.com/lusee-night/uncrater/blob/main/uncrater/Packet_Hello.py) in the uncreater package: it takes binary blob, interprets it as `startup_hello` structure and copies all attributes over. The `startup_hello` structure is ultimatelyu defined in [`core_loop.h`](https://github.com/lusee-night/coreloop/blob/devel-2/coreloop/core_loop.h) line 134 at the time of writing. If it gets changed there, the cmake will update the pycoreloop and these changes will transparently propagate into uncrater. The uncrater has an object `Collection` which can deal with a collection of packets and interpret them as needed.







