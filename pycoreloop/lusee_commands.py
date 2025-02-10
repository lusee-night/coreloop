
          
# This file is generated by scripts/create_cmd_include.py
# It includes all the commands that are available in the RFS_Settings mode
# generated from the documentation/commands.md file.

# Top-level CDI commands
          # Master command for uC control
RFS_SETTINGS =  0x10 

# Special commands for uC control that skip the queue
RFS_SPECIAL =  0x11 

# Generate outliers in this number
CTRL_OUTLIER_NUM =  0xA0 

# Generate outliers of this amplitude in units of 1/256
CTRL_OUTLIER_AMP =  0xA1 

# Generate outliers of this many (sequential) bins
CTRL_OUTLIER_BINS =  0xA2 

# Wait for 0.1s x argument (intercepted by commander)
CTRL_WAIT =  0xE0 

# Wait until you see EOS (intercepted by commander)
CTRL_WAIT_EOS =  0xE1 



# RFS Settings commands
# Stop data acquisition.
RFS_SET_STOP =  0x00 

# Start data acquisition.
RFS_SET_START =  0x01 

# Soft reset, if arg == 0: restore stored cfg, 01 = ignore stored cfg, 02 = delete all stored cfgs  0x10 init and mark program begin
RFS_SET_RESET =  0x02 

# Sets the value of temperature alarm (in Celsius)
RFS_SET_TEMP_ALARM =  0x03 

# Waits until ARG spectra are taken (stage 3)
RFS_SET_WAIT_SPECTRA =  0x04 

# Return housekeeping data, ARG = 0 -- full housekeeping; ARG = 1 ADC statistics;
RFS_SET_HK_REQ =  0x05 

# Set ADC mode: optionally disable ADCs (bits 0-3 in arg)
RFS_SET_DISABLE_ADC =  0x06 

# Autorange ADC and then set an ADC packet
RFS_SET_RANGE_ADC =  0x07 

# Request ADC waform arg contains channel number
RFS_SET_WAVEFORM =  0x08 

# Wait arg number of ticks (10ms) before processing next CMD
RFS_SET_WAIT_TICKS =  0x09 

# Wait arg number of seconds before processing next CMD
RFS_SET_WAIT_SECS =  0x0A 

# Wait arg number of mins before processing next CMD
RFS_SET_WAIT_MINS =  0x0B 

# Wait arg number of mins before processing next CMD
RFS_SET_WAIT_HRS =  0x0C 

# Debug command (used only in debugging)
RFS_SET_DEBUG =  0x0D 

# Enable (arg>1) and disable (arg=0) heartbeat.
RFS_SET_HEARTBEAT =  0x0E 

# prepare for power cut -- mode announcing power cut 5 seconds after issue
RFS_SET_TIME_TO_DIE =  0x0F 

# Control the underlying FW interpacket delay (1.28ms)
RFS_SET_CDI_FW_DLY =  0x11 

# Control the delay between spectral packets
RFS_SET_CDI_SW_DLY =  0x12 

# This writes a register through uC. First command resets value to zero
RFS_SET_WR_ADR_LSB =  0x13 

# ADR writes adreres, VAL writes value from LSB to MSB
RFS_SET_WR_ADR_MSB =  0x14 

# Val bits 0-7
RFS_SET_WR_VAL_0 =  0x15 

# Val bits 8-15
RFS_SET_WR_VAL_1 =  0x16 

# Val bits 16-23
RFS_SET_WR_VAL_2 =  0x17 

# Val bits 24-32. This triggers the actual register write
RFS_SET_WR_VAL_3 =  0x18 

# RFS_SPECIAL only! Marks beginnig of the sequence. Nothing will be executed unti SEQ_END
RFS_SEQ_START =  0x20 

# RFS_SPECIAL only! Marks end of the sequence. If ARG>0, sequence will be stored to flash and recovered on reboot
RFS_SEQ_END =  0x21 

# RFS_SPECIAL only! Breaks execution of the sequence.
RFS_SEQ_BREAK =  0x22 

# Marks beginning of a loop with ARG1 (see below)
RFS_SET_LOOP_START =  0x22 

# Marks end of repeatitions with (ARG<<8 + ARG). If 0 => infinite loop (broken by 0x11)
RFS_SET_LOOP_END =  0x23 

# Send the sequence over command once all buffers are empty.
RFS_SET_SEQ_OVER =  0x24 

# set analog gains, DD is 4x2 bits for for channels, each 2 bits encodeds L, M, H, A
RFS_SET_GAIN_ANA_SET =  0x30 

# automatic analog gains setting, min ADC. Low 2 bits are channels, remaming bits will be multiplied by 16 (1024 max val)
RFS_SET_GAIN_ANA_CFG_MIN =  0x31 

# automatic analog gains setting, max ADC = min ADC  * mult. Low 2 bits are channels, remaming bits are multiplier.
RFS_SET_GAIN_ANA_CFG_MULT =  0x32 

# Sets manual bitslicing for XCOR 1-8 (3 LSB bits) to values 1-32 (5 MSB bits)
RFS_SET_BITSLICE_LOW =  0x33 

# Sets manual bitslicing for XCOR 9-16 (3 LSB bits) to values 1-32 (5 MSB bits)
RFS_SET_BITSLICE_HIGH =  0x34 

# Uses automatic bitslicing, 0 disables, positive number sets number of SB for lowest product
RFS_SET_BITSLICE_AUTO =  0x35 

# set routing for ADC channels 1 bits 0-2 are minus, bits 3-6 are plus, bits 7-8 are direct gain
RFS_SET_ROUTE_SET1 =  0x40 

# set routing for ADC channels 2
RFS_SET_ROUTE_SET2 =  0x41 

# set routing for ADC channels 3
RFS_SET_ROUTE_SET3 =  0x42 

# set routing for ADC channels 4
RFS_SET_ROUTE_SET4 =  0x43 

# set ADCs into a special model (0 = disable , 1 = ramp, 2= zeros, 3 = ones)
RFS_SET_ADC_SPECIAL =  0x44 

# set averaging bit shifts. Lower 4 bits of DD is for Stage1 averager, higher 4 bits is for Stage2 averager. So B9 means 2^9 stage1 averaging and 2^11 stage2 averaging
RFS_SET_AVG_SET =  0x50 

# set frequency averaging. Valid values are 01, 02, 03, 04. If 03 it averages by 4 ignoring every 4th (presumably PF infected)
RFS_SET_AVG_FREQ =  0x51 

# set notch averaging, 0 = disabled, 1=x4, 2=x16, 3=x64, 4=x256
RFS_SET_AVG_NOTCH =  0x52 

# set high priority fraction as a fraction DD/FF, low priorty = 1-high-medium
RFS_SET_AVG_SET_HI =  0x53 

# set medium priority fraction, low priority is 1-high-medium
RFS_SET_AVG_SET_MID =  0x54 

# set the output format: 0 - full 32 bits resolution; 1 4+16 bits with update packets
RFS_SET_OUTPUT_FORMAT =  0x55 

# set the output correlation mask products 0-7 (autocorrelations are 4 LSB)
RFS_SET_PRODMASK_LOW =  0x56 

# set the output correlation mask products 8-15
RFS_SET_PRODMASK_HIGH =  0x57 

# Set fraction to reject, 0 to disable rejections
RFS_SET_REJ_SET =  0x60 

# Number of bad samples to reject a spectrum
RFS_SET_REJ_NBAD =  0x61 

# Time-resolved spectra starting bin, LSB
RFS_SET_TR_START_LSB =  0x62 

# Time-resolved spectra stopping bin, LSB
RFS_SET_TR_STOP_LSB =  0x63 

# Time-resolved spectra stopping MSB (b4-7), starting MSB (b0-3)
RFS_SET_TR_ST_MSB =  0x64 

# frequency averaging
RFS_SET_TR_AVG_SHIFT =  0x65 

# Enable the calibrator, arg = mode. Use 0x10 for automatic, use 0xFF to disable
RFS_SET_CAL_ENABLE =  0x70 

# bits 0-1 Nac, bits 2-5 Nac2
RFS_SET_CAL_AVG =  0x71 

# Set the notch index (2 by default for 50+100xn kHz)
RFS_SET_CAL_NINDEX =  0x72 

# Set drift guard in units of 0.1 ppm
RFS_SET_CAL_DRIFT_GUARD =  0x73 

# Sets drift stepping in units of 0.01ppm
RFS_SET_CAL_DRIFT_STEP =  0x74 

# bits 0-3 = antenna mask
RFS_SET_CAL_ANT_EN =  0x75 

# SNR required to get a lock
RFS_SET_CAL_SNR_ON =  0x76 

# SNR required to get a lock
RFS_SET_CAL_SNR_ON_HIGH =  0x77 

# SNR required to drop from a lock
RFS_SET_CAL_SNR_OFF =  0x78 

# Nsettle
RFS_SET_CAL_NSETTLE =  0x79 

# Famouse CoRRA settinh
RFS_SET_CAL_CORRA =  0x7A 

# Even more famous CorrB setting
RFS_SET_CAL_CORRB =  0x7B 

# Start setting weights. Set the ndx (0-255)
RFS_SET_CAL_WEIGHT_NDX_LO =  0x7C 

# Start setting weights. Set the ndx+256
RFS_SET_CAL_WEIGHT_NDX_HI =  0x7D 

# Sets weigth and advances index
RFS_SET_CAL_WEIGHT_VAL =  0x7E 

# set all weights to zero.
RFS_SET_CAL_WEIGHT_ZERO =  0x7F 

# set PFB NDX (8 LSB bits)
RFS_SET_CAL_PFB_NDX_LO =  0x80 

# set PFB NDX (3 MSB bits)
RFS_SET_CAL_PFB_NDX_HI =  0x81 

# Set bitslicer setting. LSB 5 bits is the slicer setting. MSB 3 bits is the slicer reg. 0 for automatic slicer control.1
RFS_SET_CAL_BITSLICE =  0x82 

# Set zoom channels to use. Bits 0-1 for CH1 and 1-2 for ch2.
RFS_SET_ZOOM_CH =  0x90 

