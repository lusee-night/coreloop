
# SPECTROMETER MODES triggered by RFS_Settings


### 0x0X - basic commands

| 0x0M | Name                |  Description                                       |
|------|---------------------|----------------------------------------------------|                             
| 0x00 | RFS_SET_STOP        | Stop data acquisition.                             |                               
| 0x01 | RFS_SET_START       | Start data acquisition.                            | 
| 0x02 | RFS_SET_RESET       | Soft reset, if arg == 0: restore stored cfg, 01 = ignore stored cfg, 02 = delete all stored cfgs  0x10 init and mark program begin                                                                                      |
| 0x03 | RFS_SET_TEMP_ALARM  | Sets the value of temperature alarm (in Celsius)                                       |
| 0x04 | RFS_SET_WAIT_SPECTRA | Waits until ARG spectra are taken (stage 3)                                           | 
| 0x05 | RFS_SET_HK_REQ      | Return housekeeping data, ARG = 0 -- full housekeeping; ARG = 1 ADC statistics;        |
| 0x06 | RFS_SET_DISABLE_ADC | Set ADC mode: optionally disable ADCs (bits 0-3 in arg)                                |
| 0x07 | RFS_SET_RANGE_ADC   | Autorange ADC and then set an ADC packet                                               |
| 0x08 | RFS_SET_WAVEFORM    | Request ADC waform arg contains channel number                                         |
| 0x09 | RFS_SET_WAIT_TICKS  | Wait arg number of ticks (10ms) before processing next CMD                             |
| 0x0A | RFS_SET_WAIT_SECS   | Wait arg number of seconds before processing next CMD                                  |
| 0x0B | RFS_SET_WAIT_MINS   | Wait arg number of mins before processing next CMD                                     |
| 0x0C | RFS_SET_WAIT_HRS    | Wait arg number of mins before processing next CMD                                     |
| 0x0D | RFS_SET_DEBUG       | Debug command (used only in debugging)                                                 |
| 0x0E | RFS_SET_HEARTBEAT   | Enable (arg>1) and disable (arg=0) heartbeat.                                          |
| 0x0F | RFS_SET_TIME_TO_DIE | prepare for power cut -- mode announcing power cut 5 seconds after issue               |


### 0x1X - further basic commands

| 0x1M | Name               |  Description                                          |
|------|--------------------|------------------------------------------------------|                             
| 0x11 | RFS_SET_CDI_FW_DLY | Control the underlying FW interpacket delay (1.28ms) |
| 0x12 | RFS_SET_CDI_SW_DLY | Control the delay between spectral packets           |
| 0x13 | RFS_SET_WR_ADR_LSB | This writes a register through uC. First command resets value to zero
| 0x14 | RFS_SET_WR_ADR_MSB | ADR writes adreres, VAL writes value from LSB to MSB
| 0x15 | RFS_SET_WR_VAL_0   | Val bits 0-7
| 0x16 | RFS_SET_WR_VAL_1   | Val bits 8-15
| 0x17 | RFS_SET_WR_VAL_2   | Val bits 16-23
| 0x18 | RFS_SET_WR_VAL_3   | Val bits 24-32. This triggers the actual register write


### 0x2X Program flow control

| 0x2M | Name                       |  Description                                       |
|------|----------------------------|----------------------------------------------------|                             
| 0x20 | RFS_SET_SEQ_BEGIN          | RFS_SPECIAL only! Marks beginnig of the sequence. Nothing will be executed unti SEQ_END
| 0x21 | RFS_SET_SEQ_END            | RFS_SPECIAL only! Marks end of the sequence. If ARG>0, sequence will be stored to flash and recovered on reboot
| 0x22 | RFS_SET_BREAK              | RFS_SPECIAL only! Breaks execution of the sequence.  
| 0x23 | RFS_SET_LOOP_START         | Marks beginning of a loop with ARG1 repetitions. If zero, infinite repetitions
| 0x24 | RFS_SET_LOOP_NEXT          | Marks end of repeatitions 
| 0x25 | RFS_SET_SEQ_OVER           | Send the sequence over command once all buffers are empty. (so we know we are done in tests) |
| 0x26 | RFS_SET_FLASH_CLR          | Clear flash to prevent restarting old sequence 

### 0x3X Gain Settings and Bit slicing

| 0x3M | Name           |  Description                                       |
|------|----------------|----------------------------------------------------|                             
| 0x30 | RFS_SET_GAIN_ANA_SET  |set analog gains, DD is 4x2 bits for for channels, each 2 bits encodeds L, M, H, A. If not A it will also set actual gains, otherwise just arm AGC
| 0x31 | RFS_SET_GAIN_ANA_CFG_MIN | automatic analog gains setting, min ADC. Low 2 bits are channels, remaming bits will be multiplied by 16 (1024 max val)
| 0x32 | RFS_SET_GAIN_ANA_CFG_MULT | automatic analog gains setting, max ADC = min ADC  * mult. Low 2 bits are channels, remaming bits are multiplier.
| 0x33 | RFS_SET_BITSLICE_LOW | Sets manual bitslicing for XCOR 1-8 (3 LSB bits) to values 1-32 (5 MSB bits)
| 0x34 | RFS_SET_BITSLICE_HIGH | Sets manual bitslicing for XCOR 9-16 (3 LSB bits) to values 1-32 (5 MSB bits)
| 0x35 | RFS_SET_BITSLICE_AUTO | Uses automatic bitslicing, 0 disables, positive number sets number of SB for lowest product

### 0x4X Signal Routing Settings

| 0x4M | Name           |  Description                                       |
|------|----------------|----------------------------------------------------|                             
| 0x40 | RFS_SET_ROUTE_SET1   | set routing for ADC channels 1 bits 0-2 are minus, bits 3-6 are plus
| 0x42 | RFS_SET_ROUTE_SET3   | set routing for ADC channels 3
| 0x43 | RFS_SET_ROUTE_SET4   | set routing for ADC channels 4
| 0x44 | RFS_SET_ADC_SPECIAL  | set ADCs into a special model (0 = disable , 1 = ramp, 2= zeros, 3 = ones) |


### 0x5X Averaging and Output Settings

| 0x5M | Name           |  Description                                       |
|------|----------------|----------------------------------------------------|                             
| 0x50 | RFS_SET_AVG_SET       | set averaging bit shifts. Lower 4 bits of DD is for Stage1 averager MINUS 8 (new in 0x300), higher 4 bits is for Stage2 averager. So B1 means 2^9 stage1 averaging and 2^11 stage2 averaging
| 0x51 | RFS_SET_AVG_FREQ      | set frequency averaging. Valid values are 01, 02, 03, 04. If 03 it averages by 4 ignoring every 4th (presumably PF infected) 
| 0x52 | RFS_SET_AVG_NOTCH     | set notch averaging, 0 = disabled, 2=x4, 4=x16, 6=x64, needs to be even.  Add 16 if you want to disable subtraction (but want to run calibrator)
| 0x53 | RFS_SET_AVG_SET_HI    | set high priority fraction as a fraction DD/FF, low priorty = 1-high-medium
| 0x54 | RFS_SET_AVG_SET_MID   | set medium priority fraction, low priority is 1-high-medium
| 0x55 | RFS_SET_OUTPUT_FORMAT | set the output format: 0 - full 32 bits resolution; 1 4+16 bits with update packets
| 0x56 | RFS_SET_PRODMASK_LOW  | set the output correlation mask products 0-7 (autocorrelations are 4 LSB) 
| 0x57 | RFS_SET_PRODMASK_HIGH | set the output correlation mask products 8-15


### 0x6X Rejection and time averaging

| 0x6M | Name                 |  Description                                       |
|------|----------------------|----------------------------------------------------|                             
| 0x60 | RFS_SET_REJ_SET      | Set fraction to reject, 0 to disable rejections    |
| 0x61 | RFS_SET_REJ_NBAD     | Number of bad samples to reject a spectrum         |
| 0x62 | RFS_SET_TR_START_LSB | Time-resolved spectra starting bin, LSB            |
| 0x63 | RFS_SET_TR_STOP_LSB  | Time-resolved spectra stopping bin, LSB            |
| 0x64 | RFS_SET_TR_ST_MSB    |  Time-resolved spectra stopping MSB (b4-7), starting MSB (b0-3)       |
| 0x65 | RFS_SET_TR_AVG_SHIFT |  frequency averaging       |






### 0x7X Calibration Settings

| 0x7M | Name                      |  Description                                       |
|------|---------------------------|----------------------------------------------------|                             
| 0x70 | RFS_SET_CAL_ENABLE        | Enable the calibrator, arg = mode. Use 0x10 for automatic, use 0xFF to disable
| 0x71 | RFS_SET_CAL_AVG           | bits 0-1 Nac, bits 2-5 Nac2 
| 0x72 | RFS_SET_CAL_NINDEX        | Set the notch index (2 by default for 50+100xn kHz)
| 0x73 | RFS_SET_CAL_DRIFT_GUARD   | Set drift guard in units of 0.1 ppm
| 0x74 | RFS_SET_CAL_DRIFT_STEP    | Sets drift stepping in units of 0.001ppm
| 0x75 | RFS_SET_CAL_ANT_EN        | bits 0-3 = antenna mask
| 0x76 | RFS_SET_CAL_SNR_ON        | SNR required to get a lock
| 0x77 | RFS_SET_CAL_SNR_ON_HIGH   | SNR required to get a lock
| 0x78 | RFS_SET_CAL_SNR_OFF       | SNR required to drop from a lock 
| 0x79 | RFS_SET_CAL_NSETTLE       | Nsettle
| 0x7A | RFS_SET_CAL_CORRA_LSB     | Famous CoRRA setting
| 0x7B | RFS_SET_CAL_CORRA_MSB     | Famous CoRRA setting (upper 8 bits, commits)
| 0x7C | RFS_SET_CAL_CORRB_LSB     | Even more famous CorrB setting
| 0x7D | RFS_SET_CAL_CORRB_MSB     | Even more famous CorrB setting (upper 8 bits, commits)
| 0x7E | RFS_SET_CAL_WEIGHT_NDX_LO | Start setting weights. Set the ndx (0-255)
| 0x7F | RFS_SET_CAL_WEIGHT_NDX_HI | Start setting weights. Set the ndx+256
| 0x80 | RFS_SET_CAL_WEIGHT_VAL    | Sets weigth and advances index
| 0x81 | RFS_SET_CAL_WEIGHT_ZERO   | set all weights to zero.
| 0x82 | RFS_SET_CAL_PFB_NDX_LO    | set PFB NDX (8 LSB bits)
| 0x83 | RFS_SET_CAL_PFB_NDX_HI    | set PFB NDX (3 MSB bits)
| 0x84 | RFS_SET_CAL_BITSLICE      | Set bitslicer setting. LSB 5 bits is the slicer setting. MSB 3 bits is the slicer reg. 0 for automatic slicer control.
| 0x85 | RFS_SET_CAL_DDRIFT_GUARD  | Set delta drift guard. 1 = 25 units of cordic angle 
| 0x86 | RFS_SET_CAL_GPHASE_GUARD  | Set gphase guard. 1=2000 units of cordic angle 



### 0x9X spectral zoom functionality 

To enable spectral functionality, you need to enable calibrator with mode = CAL_MODE_ZOOM, set the spectral channel using RFS_SET_CAL_PFB_NDX and then set the 
input ADC channel using 0x90 command below

| 0x9M | Name                 |  Description                                       |
|------|----------------------|----------------------------------------------------|                             
| 0x90 | RFS_SET_ZOOM_CH      | Set zoom channels / prods to use. Bits 0-1 for ZCH0 and 1-2 for ZCH2, bits 3-4 for mode: 00 = auto 00, 01 = 00+11 auto, 10 = 00+11+cross
| 0x91 | RFS_SET_ZOOM_NFFT    | Set number of 64 point FFTs to do before processing next channel |
| 0x92 | RFS_SET_ZOOM_NAVG    | log 2 averaging (of NFFT chunks) before spitting out data


