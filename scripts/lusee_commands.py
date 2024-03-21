
          
# This file is generated by scripts/create_cmd_include.py
# It includes all the commands that are available in the RFS_Settings mode
# generated from the documentation/commands.md file.

# Master CMD
RFS_SETTINGS = 0x10

# Commands
          # wait mode - disable data taking
RFS_SET_STOP =  0x00 

# Start data acquisition. To start anything setup by 0x1x or 0x2x
RFS_SET_START =  0x01 

# Restores default configuration (as after boot)
RFS_SET_RESET =  0x02 

# Stores current configuration
RFS_SET_STORE =  0x03 

# Recalls configuration from previous store
RFS_SET_RECALL =  0x04 

# Return housekeeping data (replace HFS_HK_REQUEST?)
RFS_SET_HK_REQ =  0x05 

# prepare for power cut -- mode announcing power cut 5 seconds after issue
RFS_SET_TIME_TO_DIE =  0x0F 

# prepare an internal test and then return the mode 00
RFS_SET_TEST_INT =  0x10 

# prepare a short test on data (take 10s of data with gains at L, M, H for a total of 30 seconds of data with default digital gain), return to the mode 00
RFS_SET_TEST_SHORT =  0x11 

# prepare a longer test on data (a preset combination of gains and mapping of antennas to ADC inputs)
RFS_SET_TEST_LONG =  0x12 

# take sufficient data to set the analog and digital gains automatically
RFS_SET_TEST_GAIN =  0x13 

# "simple science mode" - automatic gain control and input mapping disabled
RFS_SET_SCI_1 =  0x20 

# "science mode with gain control" - science mode with automatic analog and digital gain control
RFS_SET_SCI_2 =  0x21 

# "science mode with default sequencer 1
RFS_SET_SCI_3 =  0x22 

# "science mode with default sequencer 2
RFS_SET_SCI_4 =  0x23 

# set analog gains, DD is 4x2 bits for for channels, each 2 bits encodeds L, M, H, A
RFS_SET_GAIN_ANA_SET =  0x30 

# automatic analog gains setting
RFS_SET_GAIN_ANA_CFG =  0x31 

# set digital gains modes, TBD
RFS_SET_GAIN_DIG_SET =  0x32 

# set automatic digital gains modes, TBD
RFS_SET_GAIN_DIG_CFG =  0x33 

# set routing for ADC channels 1 and 2, 4 DD bits each. First two bits are antenna1 number, second two bits are antenna2 number. If antenna1==antenna2, we are subtracting from the ground. I.e. 1101 meand A4-A2. 0101 menas A2-gronud.
RFS_SET_ROUTE_SET12 =  0x40 

# same as 0x40 but for ADC channels 3 and 4
RFS_SET_ROUTE_SET34 =  0x41 

# set averaging bit shifts. Lower 4 bits of DD is for Stage1 averager, higher 4 bits is for Stage2 averager. So B9 means 2^9 stage1 averaging and 2^11 stage2 averaging
RFS_SET_AVG_SET =  0x50 

# set outlier rejectection. DD specifies the level of rejection with 00 disabled and 10 standard outlier rejection.
RFS_SET_AVG_OUTLIER =  0x51 

# set frequency averaging. Valid values are 01, 02, 03, 04. If 03 it averages by 4 ignoring every 4th (presumably PF infected)
RFS_SET_AVG_FREQ =  0x52 

# set high priority fraction as a fraction DD/FF, low priorty = 1-high-medium
RFS_SET_AVG_SET_HI =  0x53 

# set medium priority fraction, low priority is 1-high-medium
RFS_SET_AVGI_SET_MID =  0x54 

# set averaging fractions for calibration signal acquisition. Same as 0x50, but note that not all values are valid
RFS_SET_CAL_FRAC_SET =  0x60 

# set max drift guard in units of 0.1ppm
RFS_SET_CAL_MAX_SET =  0x61 

# set lock drift guard in units of 0.01ppm
RFS_SET_CAL_LOCK_SET =  0x62 

# set snr required for lock
RFS_SET_CAL_SNR_SET =  0x63 

# set starting bin (/(2*4))
RFS_SET_CAL_BIN_ST =  0x64 

# set end bin (/(2*4))
RFS_SET_CAL_BIN_EN =  0x65 

# set antenna mask as the lower 4 bits. 0x00001111 = all antennas enabled
RFS_SET_CAL_ANT_MASK =  0x66 

# enable zoom channel
RFS_SET_ZOOM_EN =  0x70 

# set zoom 1 input channel
RFS_SET_ZOOM_SET1 =  0x71 

# set zoom 1 spectral channel low bits
RFS_SET_ZOOM_SET1_LO =  0x72 

# set zoom 1 spectral channel high bits
RFS_SET_ZOOM_SET1_HI =  0x73 

# set zoom 2 input channel
RFS_SET_ZOOM_SET2 =  0x74 

# set zoom 2 spectral channel# low bits
RFS_SET_ZOOM_SET2_LO =  0x75 

# set zoom 2 spectral channel# high bits
RFS_SET_ZOOM_SET2_HI =  0x76 

# enable (DD>0), disable sequencer  (DD=0)
RFS_SET_SEQ_EN =  0xA0 

# set number of of cycle repetitions, 00 for infinite repetitions
RFS_SET_SEQ_REP =  0xA1 

# set number of elements in a cycle, restart save counter
RFS_SET_SEQ_CYC =  0xA2 

# store current configuration, as the next cycle. Store configuration includes settings under 0x30, 0x31, 0x32, 0x33, 0x40, 0x41, 0x50, 0x51, 0x52. DD means the number of integrations under this cycle
RFS_SET_SEQ_STO =  0xA3 

