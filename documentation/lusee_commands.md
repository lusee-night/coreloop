
# SPECTROMETER MODES triggered by RFS_Settings


### 0x0X - trigger modes

| 0x0M | Name                |  Description                                       |
|------|---------------------|----------------------------------------------------|                             
| 0x00 | RFS_SET_STOP        | wait mode - disable data taking                         |                              | 
| 0x01 | RFS_SET_START       | Start data acquisition. To start anything setup by 0x1x or 0x2x                        | 
| 0x02 | RFS_SET_RESET       | Soft reset, restores default configuration (as after boot)                             |
| 0x03 | RFS_SET_STORE       | Stores current configuration                                                           |
| 0x04 | RFS_SET_RECALL      | Recalls configuration from previous store                                              |
| 0x05 | RFS_SET_HK_REQ      | Return housekeeping data, ARG = 0 -- full housekeeping; ARG = 1 ADC statistics;        |
| 0x06 | RFS_SET_DISABLE_ADC | Set ADC mode: optionally disable ADCs (bits 0-3 in arg)                                |
| 0x07 | RFS_SET_RANGE_ADC   | Autorange ADC and then set an ADC packet                                               |
| 0x08 | RFS_SET_WAVEFORM    | Request ADC waform arg contains channel number                                         |
| 0x0F | RFS_SET_TIME_TO_DIE | prepare for power cut -- mode announcing power cut 5 seconds after issue               |

### 0x1X Stored sequencer modes


| 0x1M | Name               |  Description                                       |
|------|--------------------|----------------------------------------------------|                             
| 0x10 | RFS_SET_TEST       | Test modes stored internally, by number in argument
| 0x11 | RFS_SET_SCIENCE    | Science modes stored internally, by number in argument 
| 0x12 | RFS_SET_LOAD_FL    | Load sequencer mode from flash
| 0x13 | RFS_SET_STORE_FL   | Store sequencer mode into flash


### 0x3X Gain Settings and Bit slicing

| 0x3M | Name           |  Description                                       |
|------|----------------|----------------------------------------------------|                             
| 0x30 | RFS_SET_GAIN_ANA_SET  |set analog gains, DD is 4x2 bits for for channels, each 2 bits encodeds L, M, H, A
| 0x31 | RFS_SET_GAIN_ANA_CFG_MIN | automatic analog gains setting, min ADC. Low 2 bits are channels, remaming bits will be multiplied by 16 (1024 max val)
| 0x32 | RFS_SET_GAIN_ANA_CFG_MULT | automatic analog gains setting, max ADC = min ADC  * mult. Low 2 bits are channels, remaming bits are multiplier.
| 0x33 | RFS_SET_BITSLICE_LOW | Sets manual bitslicing for XCOR 1-8 (3 LSB bits) to values 1-32 (5 MSB bits)
| 0x34 | RFS_SET_BITSLICE_HIGH | Sets manual bitslicing for XCOR 9-16 (3 LSB bits) to values 1-32 (5 MSB bits)
| 0x35 | RFS_SET_BITSLICE_AUTO | Uses automatic bitslicing, 0 disables, positive number sets number of SB for lowest product

### 0x4X Signal Routing Settings

| 0x4M | Name           |  Description                                       |
|------|----------------|----------------------------------------------------|                             
| 0x40 | RFS_SET_ROUTE_SET1   | set routing for ADC channels 1 bits 0-2 are minus, bits 3-6 are plus, bits 7-8 are direct gain
| 0x41 | RFS_SET_ROUTE_SET2   | set routing for ADC channels 2
| 0x42 | RFS_SET_ROUTE_SET3   | set routing for ADC channels 3
| 0x43 | RFS_SET_ROUTE_SET4   | set routing for ADC channels 4


### 0x5X Averaging and Output Settings

| 0x5M | Name           |  Description                                       |
|------|----------------|----------------------------------------------------|                             
| 0x50 | RFS_SET_AVG_SET      | set averaging bit shifts. Lower 4 bits of DD is for Stage1 averager, higher 4 bits is for Stage2 averager. So B9 means 2^9 stage1 averaging and 2^11 stage2 averaging
| 0x51 | RFS_SET_AVG_OUTLIER  | set outlier rejectection. DD specifies the level of rejection with 00 disabled and 10 standard outlier rejection.
| 0x52 | RFS_SET_AVG_FREQ     | set frequency averaging. Valid values are 01, 02, 03, 04. If 03 it averages by 4 ignoring every 4th (presumably PF infected) 
| 0x53 | RFS_SET_AVG_NOTCH    | set notch averaging, 0 = disabled, 1=x4, 2=x16, 3=x64, 4=x256 
| 0x54 | RFS_SET_AVG_SET_HI   | set high priority fraction as a fraction DD/FF, low priorty = 1-high-medium
| 0x55 | RFS_SET_AVGI_SET_MID | set medium priority fraction, low priority is 1-high-medium
| 0x56 | RFS_SET_OUTPUT_FORMAT| set the output format: 0 - full 32 bits resolution; 1 4+16 bits with update packets


### 0x6X Calibration Settings

| 0x6M | Name           |  Description                                       |
|------|----------------|----------------------------------------------------|                             
| 0x60 | RFS_SET_CAL_FRAC_SET | set averaging fractions for calibration signal acquisition. Same as 0x50, but note that not all values are valid
| 0x61 | RFS_SET_CAL_MAX_SET  | set max drift guard in units of 0.1ppm
| 0x62 | RFS_SET_CAL_LOCK_SET | set lock drift guard in units of 0.01ppm
| 0x63 | RFS_SET_CAL_SNR_SET  | set snr required for lock 
| 0x64 | RFS_SET_CAL_BIN_ST   | set starting bin (/(2*4)) 
| 0x65 | RFS_SET_CAL_BIN_EN   | set end bin (/(2*4))
| 0x66 | RFS_SET_CAL_ANT_MASK   | set antenna mask as the lower 4 bits. 0x00001111 = all antennas enabled

### 0x7X spectral zoom functionality 

| 0x7M | Name                 |  Description                                       |
|------|----------------------|----------------------------------------------------|                             
| 0x70 | RFS_SET_ZOOM_EN      | enable zoom channel
| 0x71 | RFS_SET_ZOOM_SET1    | set zoom 1 input channel 
| 0x72 | RFS_SET_ZOOM_SET1_LO | set zoom 1 spectral channel low bits 
| 0x73 | RFS_SET_ZOOM_SET1_HI | set zoom 1 spectral channel high bits
| 0x74 | RFS_SET_ZOOM_SET2    | set zoom 2 input channel 
| 0x75 | RFS_SET_ZOOM_SET2_LO | set zoom 2 spectral channel# low bits 
| 0x76 | RFS_SET_ZOOM_SET2_HI | set zoom 2 spectral channel# high bits

### 0x8X - 0xFX reserved for future use


### 0xAX sequencer settings

| 0xAM | Name           |  Description                                       |
|------|----------------|----------------------------------------------------|                             
| 0xA0 | RFS_SET_SEQ_EN      | enable (DD>0), disable sequencer  (DD=0)
| 0xA1 | RFS_SET_SEQ_REP     | set number of of cycle repetitions, 00 for infinite repetitions
| 0xA2 | RFS_SET_SEQ_CYC     | set number of elements in a cycle, restart save counter
| 0xA3 | RFS_SET_SEQ_STO     | store current configuration, as the next cycle. Store configuration includes settings under 0x30, 0x31, 0x32, 0x33, 0x40, 0x41, 0x50, 0x51, 0x52. DD means the number of integrations under this cycle

Example: Say we want to have a 40s integration at full resoltion and two 20s integrations at half frequency resolution

| Seqeunce | Command | Effect
|----------|---------|----------
| 0 | 0xA100 | infinite repetitions
| 1 | 0xA202 | 2 cycles
| 2 | 0x50B9 | 40 second integration
| 3 | 0x5200 | no frequency integration
| 4 | 0xA301 | do it once
| 5 | 0x0A9  | 20 second intgratoin
| 6 | 0x5201 | x2 frequency integration
| 7 | 0xA302 | Do it twice


